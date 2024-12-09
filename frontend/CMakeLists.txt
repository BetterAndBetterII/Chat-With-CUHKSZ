cmake_minimum_required(VERSION 3.10)
project(Chat_With_CUHKSZ_Frontend)

set(CMAKE_CXX_STANDARD 20)

# 启用 Qt 的自动 MOC 功能
set(CMAKE_AUTOMOC ON)

# 禁用 vcpkg 中的 OpenSSL 查找功能
set(CMAKE_DISABLE_FIND_PACKAGE(OpenSSL TRUE))

# 指定 OpenSSL 的路径aa
set(OPENSSL_ROOT_DIR "D:/OpenSSL-Win64")
set(OPENSSL_INCLUDE_DIR "D:/OpenSSL-Win64/include")
set(OPENSSL_LIBRARIES "D:/OpenSSL-Win64/lib")

# 手动设置 OpenSSL 的 Crypto 和 SSL 库
set(OPENSSL_CRYPTO_LIBRARY "D:/OpenSSL-Win64/lib/VC/x64/MD/libcrypto.lib")
set(OPENSSL_SSL_LIBRARY "D:/OpenSSL-Win64/lib/VC/x64/MD/libssl.lib")

# 查找 OpenSSL
find_package(OpenSSL REQUIRED)
if (OpenSSL_FOUND)
    message(STATUS "Found OpenSSL!")
    message(STATUS "OpenSSL Include Dir: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL Libraries: ${OPENSSL_LIBRARIES}")
else ()
    message(FATAL_ERROR "Could not find OpenSSL!")
endif ()

# 查找 Qt5
#set(CMAKE_PREFIX_PATH "D:/Qt/Qt12/5.12.12/mingw73_64")
find_package(Qt5 5.12 REQUIRED COMPONENTS Core Gui Widgets)
if (Qt5_FOUND)
    message(STATUS "Qt version: ${Qt5Core_VERSION_STRING}")
    message(STATUS "Qt location: ${Qt5_DIR}")
else()
    message(FATAL_ERROR "Could not find Qt!")
endif ()


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
        ${OPENSSL_LIBRARIES}
)

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
