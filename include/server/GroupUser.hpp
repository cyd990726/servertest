#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "User.hpp"

//groupUser继承自User,但多了USer没有的属性，比如群号和群描述
class GroupUser: public User{
    private:
        int groupId;
        string groupDesc;
    public:
        GroupUser(int id=-1, string name="", string pwd="", string state="offline",
         int groupId=-1, string grouDesc=""):User(id, name, pwd, state), groupId(groupId), groupDesc(grouDesc){

        }
        int getGroupId(){
            return this->groupId;
        }
        string getGroupDesc(){
            return this->groupDesc;
        }

        void setGroupId(int groupId){
            this->groupId = groupId;
        }
        void setGroupDesc(string groupDesc){
            this->groupDesc = groupDesc;
        }

};
#endif