//
// Created by yf on 11/14/24.
//
#include "../include/Agent/Agent.h"
#include<iostream>
int main() {
    Agent agent(0);
    while(true) {
        std::string user_input;
        getline(std::cin,user_input);
        if(user_input=="EXIT") {
            break;
        }
        std::string response=agent.handler(user_input);
        std::cout<<response<<std::endl;
    }
}