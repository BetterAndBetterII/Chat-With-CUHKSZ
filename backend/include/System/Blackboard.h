//
// Created by Gary on 24-10-24.
//

#ifndef CHAT_WITH_CUHKSZ_BLACKBOARD_H
#define CHAT_WITH_CUHKSZ_BLACKBOARD_H

//#include "../../include/System/System.h" 
#include "../../include/System/CurlGlobal.h"
#include <curl/curl.h>
#include <set>

class BlackBoardSystem{
private:
    std::string username;
    std::string password;
    std::set<std::string> command_list;
    CURL* bb_handle;
    CurlGlobal curl_global_manager;
    bool is_login;

    //callback functions
    static size_t ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata);


public:
    // 构造函数声明
    BlackBoardSystem(const std::string& username, const std::string& password);
    //析构函数
    ~BlackBoardSystem();
    //返回command_list
    std::string show_commands();
    //运行指定command
    std::string execute_command(const std::string& command);
    //登录
    bool login();

};


#endif //CHAT_WITH_CUHKSZ_BLACKBOARD_H
