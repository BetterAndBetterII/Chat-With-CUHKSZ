//
// Created by Gary on 24-10-24.
//

#ifndef CHAT_WITH_CUHKSZ_BLACKBOARD_H
#define CHAT_WITH_CUHKSZ_BLACKBOARD_H



#include "../../include/System/System.h"

class BlackBoardSystem : public System {
private:
    std::string username;
    std::string password;
    bool isLogin{};

    bool login();

    std::string getLoginSession();

public:
    // 构造函数声明，按引用传递参数
    BlackBoardSystem(std::string username, std::string password);

    // 执行命令
    std::string execute_command(std::string& command) override;

    // 命令列表
    std::vector<std::string> command_list;
};


#endif //CHAT_WITH_CUHKSZ_BLACKBOARD_H
