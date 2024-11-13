//
// Created by yf on 11/13/24.
//

#include "../../include/Agent/Agent.h"
int main() {
    Model model;
    while(true){
        string user_input;
            getline(cin,user_input);
            if(user_input=="EXIT") {
                return 0;
            }
            string response=model.get_response(user_input);
            cout<<response<<endl;
        }
}
//history,
