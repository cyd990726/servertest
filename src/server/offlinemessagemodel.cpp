#include "offlinemessagemodel.hpp"
#include "db.hpp"

//插入离线消息
bool OfflineMsgModel::insert(int id, const string &msg){
    char sql[1024]={0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')", id, msg.c_str());

    MySql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            //插入成功
            return true;
        }
    }
    return false;
}

//删除对应用户的离线消息
bool OfflineMsgModel::remove(int id){
    char sql[1024]={0};
    sprintf(sql, "delete from offlinemessage where userid='%d'", id);
    
    MySql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            //删除成功
            return true;
        }
    }
    return false;
}

//查询用户的所有离线消息
vector<string> OfflineMsgModel::queryMsg(int id){
    vector<string> msgs;
    char sql[1024]={0};
    sprintf(sql, "select message from offlinemessage where userid='%d'", id);
    
    MySql mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            //从res中取出查询到的数据
            MYSQL_ROW row;
            while ((row=mysql_fetch_row(res)) != nullptr)
            {
                msgs.push_back(row[0]);
            }
            //释放掉res指向的内存的空间，要不然会发生内存泄漏
            mysql_free_result(res);
        }
    }
    return msgs;
    
}