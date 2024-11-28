#include "../include/Agent/Agent.h"
#include<iostream>
int main() {
    Agent agent;
    while(true) {
        std::string user_input;
        getline(std::cin,user_input);
        if(user_input=="EXIT") {
            break;
        }
        std::string response=agent.handler(user_input,"");
        std::cout<<response<<std::endl;
    }
}