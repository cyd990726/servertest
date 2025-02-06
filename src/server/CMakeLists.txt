#定义一个变量，包含该目录下的所有文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)

# 可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST})

#指定链接依赖的库文件
target_link_libraries(ChatServer muduo_net muduo_base pthread mysqlclient)