//
// Created by yf on 11/13/24.
//

#include "../../include/Agent/Agent.h"
int main() {
    Model model;
    while(true){
        std::string user_input;
            getline(std::cin,user_input);
            if(user_input=="EXIT") {
                return 0;
            }
            std::string response=model.get_response(user_input);
            std::cout<<response<<std::endl;
        }
}
//history,
