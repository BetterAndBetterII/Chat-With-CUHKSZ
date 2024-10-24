cmake_minimum_required(VERSION 3.10)
project(Chat_With_CUHKSZ_Frontend)

set(CMAKE_CXX_STANDARD 20)

add_executable(Chat_With_CUHKSZ_Frontend ../main.cpp)

# 包含后端的头文件
include_directories(include)

# 查找后端的源文件
file(GLOB_RECURSE BACKEND_SOURCES "src/*.cpp")

# 添加后端的可执行文件
add_executable(backend_executable ${BACKEND_SOURCES})

# 链接需要的库（例如网络库、数据库库等）
#target_link_libraries(backend_executable
#        pthread  # Linux 系统上的线程库示例
#        # 其他库
#)
