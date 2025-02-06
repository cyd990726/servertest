#include "UserModel.hpp"
#include "db.hpp"

bool UserModel::insert(User &user){
    //组装sql语句
    char sql[1024]={0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')", user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());

    MySql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            //获取插入的用户的主键id
            int id = mysql_insert_id(mysql.getConnection());
            //插入user中
            user.setID(id);
            return true;
        }
    }
    return false;

}


User UserModel::query(int id){
    User user;
    char sql[1024]={0};
    sprintf(sql, "select * from user where id=%d", id);
    MySql mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            //查询成功,以行的形式把数据取出来
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr){
                //说明查询出来的有数据
                user.setID(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);

                //最后记得释放资源，否则的话这里每查询一次就会泄露一次资源
                mysql_free_result(res);
            }
        }         
    }
    return user;

}

//更新用户状态
bool UserModel::updateState(User &user){
    char sql[1024]={0};
    
    sprintf(sql, "update user set state='%s' where id=%d", user.getState().c_str(), user.getID());
    MySql mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            //更新成功，返回true
            return true;
        }
    }
    return false;
   
}

void UserModel::resetState(){
    char sql[1024]={0};
    sprintf(sql, "update user set state='offline' where state='online'");
    MySql mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
  
}