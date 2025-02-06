#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int sockfd; // Socket文件描述符
    struct sockaddr_in server_addr; // 服务器地址结构
    char buffer[1024]; // 用于存储消息的缓冲区

    // 获取用户输入的IP地址和端口号
    string ip="127.0.0.1";
    int port = 6000;

    // 创建Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "Error creating socket." << endl;
        return -1;
    }

    // 设置服务器地址
    memset(&server_addr, 0x00, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // 将IP地址从字符串转换为网络字节序
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        cerr << "Invalid address/Address not supported." << endl;
        close(sockfd);
        return -1;
    }

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed." << endl;
        close(sockfd);
        return -1;
    }

    cout << "Connected to server." << endl;

    // 发送消息
    string message;
    while (true) {
        cout << "Enter message to send (or 'exit' to quit): ";
        getline(cin, message);

        if (message == "exit") {
            break;
        }

        // 发送消息到服务器
        send(sockfd, message.c_str(), message.size(), 0);

        // 接收服务器的响应
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0) {
            cerr << "Error receiving data." << endl;
            break;
        }

        cout << "Server response: " << buffer << endl;
    }

    // 关闭Socket
    close(sockfd);
    cout << "Disconnected from server." << endl;

    return 0;
}