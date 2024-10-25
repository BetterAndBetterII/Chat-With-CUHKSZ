//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/System/Blackboard.h"
#include <httplib.h>

int main() {
    std::cout << "Testing BlackBoard~" << std::endl;
    std::string username = "Gary";
    std::string password = "12345678";
    auto *bb = new BlackBoardSystem(username, password);
    std::string command = "command0";
    std::cout << bb->execute_command(command) << std::endl;
    command = "command2";
    std::cout << bb->execute_command(command) << std::endl;
    command = "command1";
    std::cout << bb->execute_command(command) << std::endl;

    std::cout << "Testing httplib.h" << std::endl;
     httplib::Client cli("http://www.google.com");
     //重定向
     cli.set_follow_location(true);

    // 发起 GET 请求
    if (auto res = cli.Get("/")) {
        
        // 检查响应状态
        if (res->status == 200) {
            std::cout << "Response headers:\n";
            // 打印响应头
            for (const auto& header : res->headers) {
                std::cout << header.first << ": " << header.second << "\n";
            }
        } else {
            std::cerr << "Error: " << res->status << "\n";
        }
    } else {
        std::cerr << "Request failed\n";
    }

    return 0;
}
