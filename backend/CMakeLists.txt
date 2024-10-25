cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(backend_executable)
# 包含后端的头文件
include_directories(include)

# 查找后端的源文件
file(GLOB_RECURSE BACKEND_SOURCES "src/*.cpp")

#OpenSSL support via cmake
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    set(HTTPLIB_IS_USING_OPENSSL TRUE)
endif()


# 添加后端的可执行文件
add_executable(backend_executable ./test/test_blackboard.cpp
        ${BACKEND_SOURCES})

# 链接需要的库（例如网络库、数据库库等）
#target_link_libraries(backend_executable
#        pthread  # Linux 系统上的线程库示例
#        # 其他库
#)

#The cpp-httplib library also needs a few linker flags and compile options
target_link_libraries(${PROJECT_NAME} PUBLIC
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:OpenSSL::SSL>
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:OpenSSL::Crypto>)

target_compile_definitions(${PROJECT_NAME} PUBLIC
        $<$<BOOL:${HTTPLIB_IS_USING_OPENSSL}>:CPPHTTPLIB_OPENSSL_SUPPORT>
        )

