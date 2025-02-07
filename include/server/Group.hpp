#ifndef GROUP_H
#define GROUP_H
#include <string>
using namespace std;
class Group{
private:
    int id;
    string groupName;
    string groupDesc;
public:
    Group(int id = -1, string groupname="", string groupdesc = ""):id(id), groupName(groupname),groupDesc(groupdesc){}
    int getId(){
        return this->id;
    }
    void setId(int id){
        this->id = id;
    }
    string getGroupName(){
        return this->groupName;
    }
    void setGroupName(string groupName){
        this->groupName = groupName;
    }
    string getGroupDesc(){
        return this->groupDesc;
    }
    void setGroupDesc(string groupDesc){
        this->groupDesc = groupDesc;
    }
};

#endif
