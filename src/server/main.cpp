#include "ChatServer.hpp"
#include <signal.h>
#include <unistd.h>
#include<ChatService.hpp>

void resetHandler(int sig){
    //重置所有连接的状态
    ChatService::getInstance()->reset();
    exit(EXIT_SUCCESS);
}

int main(){
    //设置信号处理函数，处理ctrl+c和abort
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGABRT);

    //先阻塞信号，避免在注册处理函数之前受到信号
    sigprocmask(SIG_BLOCK, &mask, NULL);

    //注册
    signal(SIGINT, resetHandler);
    signal(SIGABRT, resetHandler);
    //解除阻塞
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    

    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "陈炎东的服务器");
    server.start();
    loop.loop();

    return 0;
}