//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/Server/Server.h"

int main() {
    Server server;
    std::cout << "Server started" << std::endl;
    server.start();
    return 0;
}
