//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/System/Blackboard.h"

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
    return 0;
}
