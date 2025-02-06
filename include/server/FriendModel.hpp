#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H
#include "User.hpp"

class FriendModel
{

public:
    bool insert(int userid, int friendid);
    //查询用户好友列表
    vector<User> query(int userid);
    
};

#endif

