cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(Chat_With_CUHKSZ)

# 添加后端子项目
add_subdirectory(backend)
add_subdirectory(frontend)
