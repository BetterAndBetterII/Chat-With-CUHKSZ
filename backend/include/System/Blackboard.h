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

    struct Memory {
    char* response;
    size_t size;
    };

    //callback functions
    static size_t ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t write_callback(char *data, size_t size, size_t nmemb, void *clientp);

    //private function
    std::string getRequest(const std::string& url);

    std::string xpathQuery(const std::string& xmlContent, const std::string& xpathExpr);

    std::string get_course();


public:
    // 构造函数声明
    BlackBoardSystem(const std::string& username, const std::string& password);
    //析构函数
    ~BlackBoardSystem();
    //返回command_list
    std::string get_commands();
    //运行指定command
    std::string execute_command(const std::string& command);
    //登录
    bool login();

    bool change_info(const std::string& username, const std::string& password);
    //若未登录，修改账号密码并返回true，若已登录则不会修改账号密码，返回false

};


#endif //CHAT_WITH_CUHKSZ_BLACKBOARD_H
