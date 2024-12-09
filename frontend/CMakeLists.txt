cmake_minimum_required(VERSION 3.10)
project(Chat_With_CUHKSZ_Frontend)

set(CMAKE_CXX_STANDARD 20)

# 启用 Qt 的自动 MOC 功能
set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.12 REQUIRED COMPONENTS Core Gui Widgets)


# 设置源文件目录（不包含测试文件）
file(GLOB_RECURSE FRONTEND_SOURCES
        "src/*.cpp"
)

# 创建前端库
add_library(frontend_lib
        ${FRONTEND_SOURCES}
)

# 设置包含目录
target_include_directories(frontend_lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${OPENSSL_INCLUDE_DIR}
)

# 链接依赖库
target_link_libraries(frontend_lib
        PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

# 如果是Windows系统，则需要链接ws2_32
if (WIN32)
    target_link_libraries(frontend_lib PUBLIC ws2_32)
endif()

# ----------------- 部署 -----------------
add_executable(frontend src/main.cpp)
target_link_libraries(frontend
        PRIVATE
        frontend_lib
)

# ----------------- 测试 -----------------
add_executable(frontend_test src/main.cpp)
target_link_libraries(frontend_test
        PRIVATE
        frontend_lib
)
