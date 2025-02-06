#ifndef DB_H
#define DB_H
#include <mysql/mysql.h>
#include <string>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace std;

// 数据库配置信息
static const std::string server = "chenyandon.xyz";
static const std::string user = "cyd";
static const std::string password = "Cyd19990726";
static const std::string dbname = "chat";

// 数据库操作类
class MySql {
public:
    // 初始化数据库连接
    MySql();

    // 释放数据库连接资源
    ~MySql();

    // 连接数据库
    bool connect();
    // 更新操作
    bool update(const std::string& sql);

    // 查询操作
    MYSQL_RES* query(const std::string& sql);

    //获取连接
    MYSQL * getConnection();

private:

    MYSQL *_conn;
};



#endif