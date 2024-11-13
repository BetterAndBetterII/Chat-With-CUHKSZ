cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(backend_executable)
# 包含后端的头文件
include_directories(${PROJECT_SOURCE_DIR}/include)

#vcpkg包管理器
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
#set(CMAKE_TOOLCHAIN_FILE "include/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
#set(VCPKG_FEATURE_FLAGS "manifests")

# 查找后端的源文件
file(GLOB_RECURSE BACKEND_SOURCES "src/*.cpp")

# 添加后端的可执行文件
add_executable(backend_executable ${BACKEND_SOURCES})

# 链接需要的库（例如网络库、数据库库等）
find_package(cpr CONFIG REQUIRED)
target_link_libraries(backend_executable PRIVATE cpr::cpr)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(backend_executable PRIVATE nlohmann_json::nlohmann_json)