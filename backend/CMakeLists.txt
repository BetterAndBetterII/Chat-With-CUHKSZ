cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(backend)

# 查找并链接外部库
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(LibXml2 CONFIG REQUIRED)

# 查找源文件（不包含测试文件）
file(GLOB_RECURSE BACKEND_SOURCES
        "src/*.cpp"
)

# 自动查找并添加所有 include 目录
file(GLOB_RECURSE INCLUDE_DIRS "include/*" "third_party/*")
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include ${INCLUDE_DIRS})

# 创建后端库而不是可执行文件
add_library(backend_lib
        ${BACKEND_SOURCES}
)

# 设置包含目录
target_include_directories(backend_lib
        PUBLIC
        ${INCLUDE_DIRS}
)

# 将外部库链接到后端库
target_link_libraries(backend_lib
        PUBLIC
        cpr::cpr
        nlohmann_json::nlohmann_json
        curl::curl
        LibXml2::LibXml2
)

# ----------------- 测试 -----------------
# _______ 测试BB _______
add_executable(backend_test test/test_blackboard.cpp)
target_link_libraries(backend_test
        PRIVATE
        backend_lib
)

# _______ 测试Agent _______
add_executable(agent_test test/test_agent.cpp)
target_link_libraries(agent_test
        PRIVATE
        backend_lib
)

# _______ 测试Knowledge _______
add_executable(knowledge_test test/test_knowledge.cpp)
target_link_libraries(knowledge_test
        PRIVATE
        backend_lib
)

# _______ 测试BackendServer _______
add_executable(server_test src/Server/Server.cpp)
target_link_libraries(server_test
        PRIVATE
        backend_lib
)
