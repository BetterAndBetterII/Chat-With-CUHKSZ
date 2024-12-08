#include <iostream>
#include "../include/third_party/httplib.h" // 需要链接 httplib 库
#include "nlohmann/json.hpp"  // 用于解析 JSON 请求和响应
#include "../include/Server/Server.h"
using json = nlohmann::json;

void test_server() {
 //   std::cout<<"test1"<<std::endl;
    // 创建一个客户端，连接到本地服务器
    httplib::Client cli("localhost", 8080);

    // 准备请求数据
    json request_data;
    request_data["session_id"] = "session_1";
    request_data["message"] = "Hello, Server!";
    request_data["username"] = "123090337";
    request_data["password"] = "05211224Lu!";
    // 发送 POST 请求到 /chat 路径
    auto res = cli.Post("/chat", request_data.dump(), "application/json");

  //  std::cout<<res -> status <<std::endl;
    // 判断服务器响应状态
    if (res) {
        std::cout << "Test passed! Response: " << res->body << std::endl;
    } else {
        std::cout << "Test failed. Status code: " << res->status << std::endl;
    }
}

int main() {
    // 启动服务器
    Server server;
    std::thread server_thread([&]() {
        server.start();
    });

    // 等待服务器启动
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 运行测试
    test_server();

    // 停止服务器（可以根据需要退出）
    server_thread.detach();

    return 0;
}