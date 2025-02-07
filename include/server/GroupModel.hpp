#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "Group.hpp"
#include <vector>
#include <string>
#include "GroupUser.hpp"

using namespace std;
//操作群组表的类
class GroupModel
{
public:
    //创建群组
    Group creatGroup(string groupname, string groupdesc, int userid);
    //解散群组
    bool disbandGroup(int groupid);
    //退出群组
    void quitGroup(int userid, int groupid);
    //加入群
    bool joinGroup(int userid, int groupid);
    //查询群里所有成员
    vector<User> queryGroupUsers(int groupid);
    //查询用户的群数量
    vector<Group> queryGroup(int userid);
   

};

#endif