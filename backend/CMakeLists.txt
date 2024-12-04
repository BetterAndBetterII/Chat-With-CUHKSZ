cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(backend)


# 查找并链接外部库
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
# 查找libcurl
find_package(CURL CONFIG REQUIRED)
#查找Libxml
find_package(LibXml2 CONFIG REQUIRED)

# 查找源文件（不包含测试文件）
file(GLOB_RECURSE BACKEND_SOURCES
        "src/*.cpp"
)

# 创建后端库而不是可执行文件
add_library(backend_lib
        ${BACKEND_SOURCES}
)

# 设置包含目录
target_include_directories(backend_lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# 将外部库链接到后端库
target_link_libraries(backend_lib
        PUBLIC
        cpr::cpr
        nlohmann_json::nlohmann_json
        CURL::libcurl
        LibXml2::LibXml2
)

# ----------------- 部署 -----------------
add_executable(backend src/Server/Server.cpp)
target_link_libraries(backend
        PRIVATE
        backend_lib
)

# ----------------- 测试 -----------------
# _______ 测试BB _______
add_executable(blackboard_test test/test_blackboard.cpp)
target_link_libraries(blackboard_test
        PRIVATE
        backend_lib
)

# _______ 测试Booking _______
add_executable(booking_test test/test_booking.cpp)
target_link_libraries(booking_test
        PRIVATE
        backend_lib
)

# _______ 测试Email _______
add_executable(email_test test/test_email.cpp)
target_link_libraries(email_test
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
