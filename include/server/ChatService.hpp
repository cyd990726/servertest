#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <functional>
#include <muduo/net/TcpServer.h>
#include <unordered_map>
#include "public.hpp"
#include <UserModel.hpp>
#include "json.hpp"
#include <mutex>
#include <offlinemessagemodel.hpp>
#include "FriendModel.hpp"
#include "Group.hpp"
#include "GroupModel.hpp"
#include "GroupUser.hpp"

using nlohmann::json;

using namespace muduo;
using namespace muduo::net;
using namespace std;

//处理消息的回调方法类型。
using MsgHandler = function<void(const TcpConnectionPtr& conn,
                            json& js,
                            Timestamp tp)>;

class ChatService
{
private:
    //单例模式，禁止默认构造
    ChatService();

    //记录每一个msgid对应的msgHandler
    unordered_map<int, MsgHandler> _msgHandlerMap;

    //定义一个UserModel
    UserModel usermodel;
    OfflineMsgModel offlineMsgModel;
    FriendModel friendModel;
    GroupModel groupModel;

    //记录用户连接信息
    //注意：这个因为网络层会创建多个线程调用业务层的逻辑代码，userConnMap的使用需要考虑线程安全问题
    unordered_map<int, TcpConnectionPtr> userConnMap;

    //定义一个对userConnMap访问的互斥锁
    mutex connMutex;

public:
    
    //暴露一个获取单例的接口
    static ChatService * getInstance();

    //登录业务
    void login(const TcpConnectionPtr& conn,
                            json &js,
                            Timestamp tp);
    //注册业务
    void reg(const TcpConnectionPtr& conn,
                            json &js,
                            Timestamp tp);
    //一对一聊天业务
    void oneChat(const TcpConnectionPtr& conn,
                            json &js,
                            Timestamp t);
    //获取消息对应的事件处理器
    MsgHandler getHandler(int msgid);

    //客户端异常关闭
    void clientCloseException(const TcpConnectionPtr& conn);

    //服务器关闭后重置所有用户状态的函数
    void reset();

    //添加好友业务
    void addFriend(const TcpConnectionPtr& conn,
                            json &js,
                            Timestamp tp);
    
    //创建群组业务
    void createGroup(const TcpConnectionPtr& conn,
                            json &js,
                            Timestamp tp);
    
    //加入群组业务
    void joinGroup(const TcpConnectionPtr& conn,
                            json &js,
                            Timestamp tp);

};



#endif