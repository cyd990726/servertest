#include "FriendModel.hpp"
#include <string.h>
#include <iostream>
#include "db.hpp"
#include <vector>

using namespace std;

//往数据库表friend中添加好友
bool FriendModel::insert(int userid, int friendid){
    char sql[1024];
    memset(sql, 0x00, sizeof(sql));
    sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);
    MySql mysql;
    if(mysql.connect()){
        //连接成功
        if(mysql.update(sql)){
            return true;    
        }
    }
    return false;

}

//对User、Friend两个表进行联合查询，返回用户的所有好友信息
vector<User> FriendModel::query(int userid){
    vector<User> friends;
    char sql[1024];
    memset(sql, 0x00, sizeof(sql));
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid=a.id where b.userid=%d", userid);

    //定义一个连接
    MySql mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            User user;
            //查询成功,以行的形式把数据取出来
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                //说明查询出来的有数据
                user.setID(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                friends.push_back(user);
            }
            //最后记得释放资源，否则的话这里每查询一次就会泄露一次资源
            mysql_free_result(res);
        }         
    }
    return friends;

}