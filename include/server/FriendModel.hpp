#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H
#include "User.hpp"
#include <vector>
using namespace std;

class FriendModel
{

public:
    bool insert(int userid, int friendid);
    //查询用户好友列表
    vector<User> query(int userid);
    
};

#endif

