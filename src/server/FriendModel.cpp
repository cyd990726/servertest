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
    //先查询一下是不是已经存在
    sprintf(sql, "select * from friend where (userid='%d' and friendid='%d') or (friendid='%d' and userid='%d')", userid, friendid, userid, friendid);
    MySql mysql;
    if(mysql.connect()){
        //连接成功
        MYSQL_RES *res;
        if((res=mysql.query(sql))==nullptr){
            //查询失败
            return false;    
        }
        //从res里面按行取数据，试探性取一行就行
        MYSQL_ROW row = mysql_fetch_row(res);
        mysql_free_result(res);
        if(row != nullptr){
            //说明查到东西了
            return false;
        }
        //说明没查到东西，两个人间不存在好友关系
        memset(sql, 0x00, sizeof(sql));
        //新增好友
        sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);
        if(mysql.update(sql)){
            //插入成功
            return true;
        }
    }
    return false;
}


vector<User> FriendModel::query(int userid) {
    vector<User> friends;
    char sql[1024];
    snprintf(sql, sizeof(sql), 
             "SELECT a.id, a.name, a.state "
             "FROM user a "
             "INNER JOIN friend b "
             "ON (b.friendid = a.id AND b.userid = %d) "
             "OR (b.userid = a.id AND b.friendid = %d)", userid, userid);

    MySql mysql;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            User user;
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                user.setID(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                friends.push_back(user);
            }
            mysql_free_result(res);
        } 
        //查询失败了
    }
    return friends;
}