cmake_minimum_required(VERSION 3.10)
project(Chat_With_CUHKSZ_Frontend)

set(CMAKE_CXX_STANDARD 20)

# 启用Qt的自动MOC功能
#set(CMAKE_AUTOMOC ON)

# 查找并链接外部库
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
# 查找源文件（不包含测试文件）
file(GLOB_RECURSE FRONTEND_SOURCES
        "src/*.cpp"
)

# 创建前端库而不是可执行文件
add_library(frontend_lib
        ${FRONTEND_SOURCES}
)

# 设置包含目录
target_include_directories(frontend_lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# 将外部库链接到后端库
target_link_libraries(frontend_lib
        PUBLIC
#        Qt5::Core
#        Qt5::Gui
#        Qt5::Widgets
 #       cpr::cpr
        nlohmann_json::nlohmann_json
)

# ----------------- 部署 -----------------
#add_executable(frontend src/main.cpp
#)
#target_link_libraries(frontend
#        PRIVATE
#        frontend_lib
#)

# ----------------- 测试 -----------------
# _______ 测试前端 _______
#add_executable(frontend_test src/main.cpp
#)
#target_link_libraries(frontend_test
#        PRIVATE
#        frontend_lib
#)

add_executable(Client_test src/Client/Client.cpp)
target_link_libraries(Client_test
        PRIVATE
        frontend_lib

)
