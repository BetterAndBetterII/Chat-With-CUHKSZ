//
// Created by Gary on 24-10-24.
//

#ifndef CHAT_WITH_CUHKSZ_EMAIL_H
#define CHAT_WITH_CUHKSZ_EMAIL_H

//#include "../../include/System/System.h"
#include "../../include/System/CurlGlobal.h"
#include <curl/curl.h>
#include <set>
#include<vector>
#define EMAIL_POSTFIX "@link.cuhk.edu.cn"
#define SMTP_SERVER "smtp://smtp.office365.com"
#define SMTP_PORT 587

class EmailSystem{
private:
    std::string username;
    std::string password;
    std::set<std::string> command_list;
    CURL* email_handle;
    CurlGlobal curl_global_manager;
    bool is_login;

    struct Memory {
        char* response;
        size_t size;
    };

    struct UploadStatus {
        size_t bytes_read;
        EmailSystem* system;
        UploadStatus(EmailSystem* s) : bytes_read(0), system(s) {}
    };
    
    std::string email_content;  // 存储要发送的邮件内容
    
    //callback functions
    static size_t ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t write_callback(char *data, size_t size, size_t nmemb, void *clientp);

    std::vector<std::string> xpathQuery(const std::string& xmlContent, const std::string& xpathExpr);

    //创建邮件
    std::string create_message(const std::vector<std::string>& recipients_str, const std::string& subject, const std::string& body) const;

    // 声明静态成员函数
    static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp);

public:
    // 构造函数声明
    EmailSystem(const std::string& username, const std::string& password);
    //析构函数
    ~EmailSystem();
    //返回command_list
    std::string get_commands();
    //登录
    bool login();
    //更换账号密码
    bool change_info(const std::string& username, const std::string& password);

    //发送邮件
    std::string send_email(const std::vector<std::string>& recipients_str, const std::string& subject, const std::string& body);

};


#endif //CHAT_WITH_CUHKSZ_EMAIL_H
