cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(backend_executable)
# 包含后端的头文件
include_directories(include)

# 查找后端的源文件
file(GLOB_RECURSE BACKEND_SOURCES "src/*.cpp")

# 查找libcurl
find_package(CURL REQUIRED)
#查找Libxml
find_package(LibXml2 REQUIRED)


# 添加后端的可执行文件
add_executable(backend_executable 
        ./test/test_blackboard.cpp
        ${BACKEND_SOURCES})

# 链接需要的库（例如网络库、数据库库等）
#target_link_libraries(backend_executable
#        pthread  # Linux 系统上的线程库示例
#        # 其他库
#)
target_link_libraries(backend_executable PRIVATE CURL::libcurl)
target_link_libraries(backend_executable PRIVATE LibXml2::LibXml2)

