//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/System/Blackboard.h"

//Test libcurl
#include "test_libcurl.cpp"

int main() {
    std::cout << "=====Testing BlackBoard=====" << std::endl;
    std::cout << "---Testing Login---" << std::endl;
    //basic info input
    std::string username = "";
    std::string password = "";
    std::cout << "Username:" << std::endl;
    std::cin >> username;
    std::cout << "Password:" << std::endl;
    std::cin >> password;
    auto *bb = new BlackBoardSystem(username, password);
    if(bb->login()){

        std::cout << "---Testing show_command---" << std::endl;
        //test show_command
        std::cout << bb->show_commands() << std::endl;

        std::cout << "---Testing excute_command---" << std::endl;
        //test excute command
        std::string command = "command0";
        std::cout << bb->execute_command(command) << std::endl;
    }
    delete bb;

    //Test Libcurl
    //test_libcurl();

    return 0;
}
