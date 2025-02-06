#ifndef USERMODEL_H
#define USERMODEL_H
#include "User.hpp"

//这个类是用来操作User表的
class UserModel
{

public:
    //往表中插入一条数据
    bool insert(User &user);
    User query(int id);
    bool updateState(User &user);
    void resetState();
};





#endif