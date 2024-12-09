//
// Created by Gary on 24-10-23.
//

#include <vector>
#include <string>

#ifndef CHAT_WITH_CUHKSZ_SYSTEM_H
#define CHAT_WITH_CUHKSZ_SYSTEM_H


class System {
private:
    std::string username;
    std::string password;
public:
    const std::vector<std::string> command_list;
    virtual std::string execute_command(std::string& command) = 0;
};

//std::string System::execute_command(std::string& command) {
//    return {};
//}


#endif //CHAT_WITH_CUHKSZ_SYSTEM_H
