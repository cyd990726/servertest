#include "ChatServer.hpp"
#include <iostream>
#include <functional>
#include "json.hpp"
#include "ChatService.hpp"
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace std;
using nlohmann::json;

ChatServer::ChatServer(EventLoop* loop,
        const InetAddress& listenAddr,
        const string& nameArg):_server(loop, listenAddr, nameArg), _loop(loop)
{
    
    _server.setConnectionCallback(bind(&ChatServer::onConnectionCallback, this, placeholders::_1));
    _server.setMessageCallback(bind(&ChatServer::onMessageCallback, this, placeholders::_1, placeholders::_2, placeholders::_3));

    _server.setThreadNum(4);

}

ChatServer::~ChatServer()
{

}

//回调函数:链接建立、断开时会自动调用
void ChatServer::onConnectionCallback(const TcpConnectionPtr& conn)
{
    if(conn->connected()){
        //链接相关的业务，没有的话就不用写
        LOG_INFO<<conn->peerAddress().toIpPort()<<"已连接";
    }else{
        //调用业务层
        ChatService::getInstance()->clientCloseException(conn);

        //如果断开了就释放连接
        conn->shutdown();
        
    }
    

}
//收到消息的时候会调用
void ChatServer::onMessageCallback(const TcpConnectionPtr& conn,
                        Buffer* buf,
                        Timestamp tp)
{
    //这是一个json字符串
    string msg = buf->retrieveAllAsString();
    LOG_INFO<<"msg:"<<msg;
    //利用json库将字符串反序列化成一个json对象。
    json js = json::parse(msg);
    
    //通过js里面读出来的msgid调用相应的服务，不要指名道姓的调用业务模块的方法，实现网络层和业务层的解耦。

    //通过msgid得到对应的方法
    auto msgHandler = ChatService::getInstance()->getHandler(js["msgid"].get<int>());
    //再调用这个方法。
    msgHandler(conn, js, tp);

}
void ChatServer::start()
{
    _server.start();
}