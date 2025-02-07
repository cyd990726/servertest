#include "ChatService.hpp"
#include "public.hpp"
#include <functional>
#include <muduo/base/Logging.h>
#include <mutex>
using namespace muduo;
using namespace std;

//暴露一个获取单例的接口
ChatService * ChatService::getInstance(){
    //定义一个静态的service，能够确保每次调用这个函数获取到的都是同一个
    static ChatService service;
    return &service;
}

//注册消息对应的回调操作
ChatService::ChatService(){
    _msgHandlerMap.insert({EnMsgType::LOGIN_MSG, bind(&ChatService::login, this, placeholders::_1, placeholders::_2, placeholders::_3)});
    _msgHandlerMap.insert({EnMsgType::REG_MSG, bind(&ChatService::reg, this, placeholders::_1, placeholders::_2, placeholders::_3)});
    _msgHandlerMap.insert({EnMsgType::ONE_CHAT_MSG, bind(&ChatService::oneChat, this, placeholders::_1, placeholders::_2, placeholders::_3)});
    _msgHandlerMap.insert({EnMsgType::ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, placeholders::_1, placeholders::_2, placeholders::_3)});
    _msgHandlerMap.insert({EnMsgType::CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, placeholders::_1, placeholders::_2, placeholders::_3)});
    _msgHandlerMap.insert({EnMsgType::JOIN_GROUP_MSG, bind(&ChatService::joinGroup, this, placeholders::_1, placeholders::_2, placeholders::_3)});
}

//登录业务
void ChatService::login(const TcpConnectionPtr& conn,
                        json& js,
                        Timestamp tp)
{
    //先获取id、密码
    int id = js["id"].get<int>();
    string pwd = js["password"].get<string>();
    //先根据id查询一下
    User user = usermodel.query(id);
    json respond;
    respond["msgid"] = EnMsgType::LOGIN_MSG_ACK;
    //默认的id是-1，用户名是空的
    respond["id"] = user.getID();
    respond["name"] = user.getName();
    if(user.getID()!=-1 && user.getPassword()==pwd){
        //用户名密码正确
        if(user.getState()=="online"){
            //已经登陆了
            respond["errno"] = 1;
            respond["ackmsg"] = "不允许重复登录";
        }else{
            //登录成功
            respond["ackmsg"] = "登陆成功";
            //记录用户连接信息。
            {
                lock_guard<mutex> lock(connMutex);
                userConnMap.insert({id, conn});
            }
            respond["errno"] = 0;
            //设置数据库中用户的state为online
            user.setState("online");
            usermodel.updateState(user);
            //从数据库中读取离线消息
            vector<string>msgs = offlineMsgModel.queryMsg(id);
            //读完了就得删除
            if(!msgs.empty()){
                LOG_INFO<<"读到了离线消息"<<msgs.size()<<"条";
                bool ret = offlineMsgModel.remove(id);
                //把离线消息转发给用户
                respond["offlinemsg"]=msgs;
            }
            //把好友列表给用户发过去
            vector<User> friends = friendModel.query(id);
            if(!friends.empty()){
                vector<json> vec;
                for(User &u: friends){
                    json subjs;
                    subjs["id"] = u.getID();
                    subjs["name"] = u.getName();
                    subjs["state"] = u.getState();
                    vec.push_back(subjs);
                }
                respond["friends"] = vec;
            }
            
            //查询它所在的群
            vector<Group> groups = groupModel.queryGroup(id);
            if(!groups.empty()){
                vector<json> sec;
                json subjs;
                for_each(groups.begin(), groups.end(), [&](Group &g){
                    subjs["id"]=g.getId();
                    subjs["groupname"]=g.getGroupName();
                    subjs["groupdesc"]=g.getGroupDesc();
                    sec.push_back(subjs);
                });
                respond["groups"] = sec;
            }
        }  
    }else{
        //登录失败，用户名或者密码错误
        respond["errno"] = 1;
        respond["ackmsg"] = "用户名或者密码错误";
        LOG_INFO<<"登录失败";
    }

    conn->send(respond.dump());

}
//注册业务
void ChatService::reg(const TcpConnectionPtr& conn,
                        json& js,
                        Timestamp tp)
{
    //先获取用户名、密码
    string name = js["name"].get<string>();
    string pwd = js["password"].get<string>();

    User user;
    user.setName(name);
    user.setPassword(pwd);

    //插入数据
    int state = usermodel.insert(user);
    //定义一个响应，回复给客户端
    json respond;
    respond["msgid"] = EnMsgType::REG_MSG_ACK;
    respond["id"] = user.getID();
    if(state){
        //插入成功
        respond["errno"] = 0; 
    }
    else{
        //插入失败 
        respond["errno"] = 1;
    }
    //发送回去
    conn->send(respond.dump());
}

MsgHandler ChatService::getHandler(int msgid){
    //记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end()){
        //返回一个空操作的处理器
        return [=](const TcpConnectionPtr& conn,
                            json& js,
                            Timestamp tp){
                                LOG_ERROR << "msgid:"<<msgid<<"can not find handler";
                            };
        
    }
    return _msgHandlerMap[msgid];
}

void ChatService::clientCloseException(const TcpConnectionPtr& conn){
    //客户端关闭时需要做的处理
    User user;
    //先从map中删除记录的连接
    {
        lock_guard<mutex> lock(connMutex);
        for(auto it = userConnMap.begin(); it != userConnMap.end(); it++){
            if(it->second == conn){
                user.setID(it->first);
                userConnMap.erase(it);
                break;
            }
        }
    }
    //更新数据库中的状态
    if(user.getID()!=-1){

        //设置状态为离线状态
        user.setState("offline");
        //更新数据库中用户状态信息
        usermodel.updateState(user);
    }
    return ;
    
}

//一对一聊天业务。我们服务器起的是一个消息的中转站作用
void ChatService::oneChat(const TcpConnectionPtr& conn,
                        json &js,
                        Timestamp t)
{
    LOG_INFO<<"执行到了service::onechat";
    json respond;
    respond["msgid"] = EnMsgType::ONE_CHAT_MSG_ACK;
    //先把对方的id给取出来
    int toid = js["toid"].get<int>();

    
    //根据对方是否在线来进行业务的处理
    {
        lock_guard<mutex> lock(connMutex);
        auto it = userConnMap.find(toid);
        if(it != userConnMap.end()){
            //对方在线。给他发送消息
            it->second->send(js.dump());
            respond["errno"] = 0;
            respond["ackmsg"] = "对方在线,消息已经送达";
            conn->send(respond.dump());
            return;   
        }
    }
    
    //对方不在线，存储离线消息
    if(offlineMsgModel.insert(toid, js.dump())){
        respond["errno"] = 0;
        //离线消息存储成功
        respond["ackmsg"] = "对方不在线，发送为离线消息";
        conn->send(respond.dump());
        return;
    }

    //离线消息存储失败
    respond["errno"] = 1;
    respond["ackmsg"] = "离线消息存储失败";
    conn->send(respond.dump());
}

//服务器关闭后重置所有用户状态的函数
void ChatService::reset(){
    usermodel.resetState();
}

//添加好友业务
void ChatService::addFriend(const TcpConnectionPtr& conn,
                            json &js,
                            Timestamp tp)
{
    json respond;
    respond["msgid"] = EnMsgType::ADD_FRIEND_MSG_ACK;
    // 先从js对象中获取id
    int userid = js["id"].get<int>();
    int friendid = js["friend"].get<int>();
    //调用model层添加好友。
    if(friendModel.insert(userid, friendid)){
        respond["errno"] = 0;
        respond["ackmsg"] = "添加好友成功";
    }else{
        respond["errno"] = 1;
        respond["ackmsg"] = "添加好友失败";
    }
    //发送响应
    conn->send(respond.dump());
    return ;
}

//创建群组业务
void ChatService::createGroup(const TcpConnectionPtr& conn,
                        json &js,
                        Timestamp tp)
{
    //用户信息
    int userid = js["id"].get<int>();
    //要创建的群信息
    string groupdesc = js["groupdesc"];
    string groupname = js["groupname"];

    //定义响应json
    json respond;
    respond["msgid"] = EnMsgType::CREATE_GROUP_MSG_ACK;
    //开始创建群
    Group group = groupModel.creatGroup(groupname, groupdesc, userid);
    if(group.getId() == -1){
        respond["errno"] = 1;
        respond["ackmsg"]="群创建失败";
    }else{
        respond["errno"] = 0;
        respond["ackmsg"] = "群创建成功";
 
        json subjs;
        subjs["id"] = group.getId();
        subjs["groupname"] = group.getGroupName();
        subjs["groupdesc"] = group.getGroupDesc();
        respond["group"] = subjs;
    }
    
    //以json字符串的形式响应
    conn->send(respond.dump());
    return ;
}

    //加入群组业务
void ChatService::joinGroup(const TcpConnectionPtr& conn,
    json &js,
    Timestamp tp)
{
    //用户信息以及要加入的群信息
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    //定义响应json
    json respond;
    respond["msgid"] = EnMsgType::JOIN_GROUP_MSG_ACK;

    if(groupModel.joinGroup(userid, groupid)){
        respond["errno"] = 0;
        respond["ackmsg"] = "加入群组成功";
    }else{
        respond["errno"] = 1;
        respond["ackmsg"] = "加入群组失败";
    }
    //以json字符串的形式响应
    conn->send(respond.dump());
    return ;
}