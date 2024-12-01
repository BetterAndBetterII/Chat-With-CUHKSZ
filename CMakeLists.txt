cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(Chat_With_CUHKSZ)

# 添加选项控制是否构建后端
option(BUILD_BACKEND "Build backend components" ON)
option(BUILD_FRONTEND "Build frontend components" ON)

# 根据选项添加子项目
if(BUILD_BACKEND)
    add_subdirectory(backend)
endif()

if(BUILD_FRONTEND)
    add_subdirectory(frontend)
endif()