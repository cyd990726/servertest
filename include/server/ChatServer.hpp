#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <string>

using namespace muduo::net;
using namespace muduo;
using std::string;

class ChatServer
{
private:
    TcpServer _server;
    EventLoop *_loop;
public:
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
    ~ChatServer();
    //回调函数
    void onConnectionCallback(const TcpConnectionPtr& conn);
    void onMessageCallback(const TcpConnectionPtr& conn,
                            Buffer* buf,
                            Timestamp tp);
    void start();
};




#endif