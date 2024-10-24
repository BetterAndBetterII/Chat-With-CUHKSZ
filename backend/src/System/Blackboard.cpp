//
// Created by Gary on 24-10-23.
//
#include <iostream>
#include <utility>
#include "../../include/System/Blackboard.h"

// 构造函数实现，使用初始化列表初始化
BlackBoardSystem::BlackBoardSystem(std::string username, std::string password) {
    this->command_list = {"command0", "command1", "command2"};
    this->username = std::move(username);
    this->password = std::move(password);
}

// 执行命令的实现
std::string BlackBoardSystem::execute_command(std::string& command) {
    if (command == command_list[0]) {
        return "command0 executed";
    } else if (command == command_list[1]) {
        return "command1 executed";
    } else if (command == command_list[2]) {
        return "command2 executed";
    }
    return "command executed";
}

// 登录逻辑的实现
bool BlackBoardSystem::login() {
    // login
    return true;
}

// 获取登录会话
std::string BlackBoardSystem::getLoginSession() {
    if (!isLogin) {
        if (login()) {
            std::cout << "Login success" << std::endl;
            isLogin = true;
        } else {
            std::cout << "Login failed" << std::endl;
        }
    }
    std::cout << "Return login session" << std::endl;
    return "login session";
}
