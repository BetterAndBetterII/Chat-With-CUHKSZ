#include "../include/Agent/Agent.h"
#include <iostream>
int main() {
    std::string username;
    std::string password;
    std::cout<<"enter the username"<<std::endl;
    getline(std::cin,username);
    std::cout<<"enter the password"<<std::endl;
    getline(std::cin,password);
    Agent agent = Agent(username,password);
    while(true) {
        std::string user_input;
        getline(std::cin,user_input);
        if(user_input=="EXIT") {
            break;
        }
        std::string response=agent.run_until_done(user_input);
        std::cout<<response<<std::endl;
    }
}