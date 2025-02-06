#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H
#include <string>
#include <vector>

using namespace std;
//操作离线消息表类
class OfflineMsgModel{
private:


public:
    //插入离线消息
    bool insert(int id, const string &msg);

    //删除对应用户的离线消息
    bool remove(int id);
    
    //查询用户的所有离线消息
    vector<string> queryMsg(int id);


};

#endif