#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User
{
private:
    int id;
    string name;
    string password;
    string state;
public:
    User(int id=-1, string name="", string pwd="", string state="offline"):id(id), name(name), password(pwd), state(state){

    }
    void setID(int id){
        this->id = id;
    }
    void setName(string name){
        this->name = name;
    }
    void setPassword(string password){
        this->password = password;
    }
    void setState(string state){
        this->state = state;
    }

    int getID(){
        return this->id;
    }
    string getName(){
        return this->name;
    }
    string getPassword(){
        return this->password;
    }
    string getState(){
        return this->state;
    }

    ~User()=default;
};

#endif
