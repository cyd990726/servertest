#include "GroupModel.hpp"
#include<iostream>
#include "db.hpp"
#include "string.h"
#include <muduo/base/Logging.h>
#include "GroupUser.hpp"
using namespace muduo;
using namespace std;
//创建群组
Group GroupModel::creatGroup(string groupname, string groupdesc, int userid){

    Group group;
    char sql[2024] = {0};
    MySql mysql;
    if(mysql.connect()){
        //设置手动提交
        mysql.setAutoCommit(0);
        //执行第一条命令
        sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')", groupname.c_str(), groupdesc.c_str());
        // 更新操作
        if(!mysql.update(sql)){
            //创建群组失败
            mysql.rollBack();//回滚事务
            mysql.setAutoCommit(1);
            return group;
        }
        //创建群组成功
        //把groupid返回一下
        int groupid = mysql_insert_id(mysql.getConnection());

        //执行第二条命令
        memset(sql, 0x00, sizeof(sql));
        sprintf(sql, "insert into groupuser(groupid, userid, grouprole) values(%d, %d, 'creator')",groupid, userid);
        if(!mysql.update(sql)){
            //添加groupuser失败
            mysql.rollBack();//回滚事务
            mysql.setAutoCommit(1);
            return group;
        }
        //添加groupuser成功
        //提交事务:mysql_commit返回1表示提交失败，返回0表示提交成功
        if(mysql_commit(mysql.getConnection())){
            LOG_INFO<<"提交事务失败";
            mysql.setAutoCommit(1);
            return group;
        }

        group.setId(groupid);
        group.setGroupName(groupname);
        group.setGroupDesc(groupdesc);

        mysql.setAutoCommit(1);
        return group;
    }
    return group;
}
//解散群组
bool GroupModel::disbandGroup(int groupid){
    char sql[2024] = {0};
    MySql mysql;
    if(mysql.connect()){
        //先把allgroup表里的东西删干净了，再把groupuser表里的成员删除干净
        sprintf(sql, "delete from allgroup where id=%d", groupid);
        //设置手动提交
        mysql.setAutoCommit(0);
        //执行sql
        if(!mysql.update(sql)){
            //执行失败
            mysql.rollBack();//回滚
            mysql.setAutoCommit(1);
            return false;
        }
        memset(sql, 0x00, sizeof(sql));
        sprintf(sql, "delete from groupuser where groupid=%d", groupid);
        if(!mysql.update(sql)){
            //执行失败
            mysql.rollBack();//回滚
            mysql.setAutoCommit(1);
            return false;
        }
        //提交事务
        if(mysql_commit(mysql.getConnection())){
            //提交失败
            mysql.rollBack();//回滚
            mysql.setAutoCommit(1);
            return false;
        }
        //提交成功
        mysql.setAutoCommit(1);
        return true;
    }
    //说明数据库没连接上
    return false;
}
//加入群组
bool GroupModel::joinGroup(int userid, int groupid){
    return false;
}

//退出群组
void GroupModel::quitGroup(int userid, int groupid){

}

//查询群里所有成员
vector<User> queryGroupUsers(int groupid){
    char sql[2024] = {0};
    MySql mysql;

    vector<User> users;
    if(mysql.connect()){
        //查询groupuser表
        sprintf(sql, "select b.id as userid,b.name as name,b.state as state from groupuser as a inner join user as b on a.userid=b.id where a.groupid=%d", groupid);
        MYSQL_RES *res = mysql.query(sql);
        if(res!=nullptr){
            //查询成功
            MYSQL_ROW row;
            User user;
            while((row=mysql_fetch_row(res))!=nullptr){
                user.setID(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                users.push_back(user);
            }
            //释放res内存
            mysql_free_result(res);
        }
    }
    return users;
}

//查询用户的群数量
vector<Group> GroupModel::queryGroup(int userid){
    vector<Group> groups;
    char sql[2024] = {0};
    MySql mysql;
    sprintf(sql, "SELECT allgroup.id, allgroup.groupname, allgroup.groupdesc FROM allgroup INNER JOIN groupuser ON allgroup.id = groupuser.groupid WHERE groupuser.userid = %d", userid);
    if(mysql.connect()){
        //连接成功
        MYSQL_RES* res = mysql.query(sql);
        if(res!=nullptr){
            MYSQL_ROW row;
            Group group;
            while((row=mysql_fetch_row(res))!=nullptr){
                group.setId(atoi(row[0]));
                group.setGroupName(row[1]);
                group.setGroupDesc(row[2]);
                groups.push_back(group);
            }
            //释放内存
            mysql_free_result(res);
        }
    }
    return groups;

}
