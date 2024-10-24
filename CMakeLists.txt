cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
project(Chat_With_CUHKSZ)

# 添加子项目（前端和后端）
#add_subdirectory(frontend)
add_subdirectory(backend)

# 前端可执行文件
#add_executable(Chat_With_CUHKSZ ./frontend/main.cpp)
# 后端可执行文件
add_executable(Chat_With_CUHKSZ backend/test/test_blackboard.cpp)
