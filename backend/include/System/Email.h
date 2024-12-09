//
// Created by Gary on 24-10-24.
//

#ifndef CHAT_WITH_CUHKSZ_EMAIL_H
#define CHAT_WITH_CUHKSZ_EMAIL_H

#include "../../include/System/System.h"
#define EMAIL_POSTFIX "@link.cuhk.edu.cn"
#define SMTP_SERVER "smtp://smtp.office365.com"
#define SMTP_PORT 587

class EmailSystem : public System{
private:

    struct UploadStatus {
        size_t bytes_read;
        EmailSystem* system;
        UploadStatus(EmailSystem* s) : bytes_read(0), system(s) {}
    };
    
    std::string email_content;  // 存储要发送的邮件内容
    
    //创建邮件
    std::string create_message(const std::vector<std::string>& recipients_str, const std::string& subject, const std::string& body) const;

    // 声明静态成员函数
    static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp);

public:
    // 构造函数声明
    EmailSystem(const std::string& username, const std::string& password);
    //析构函数
    ~EmailSystem();
    //登录
    bool login();
    //发送邮件
    std::string send_email(const std::vector<std::string>& recipients_str, const std::string& subject, const std::string& body);

};


#endif //CHAT_WITH_CUHKSZ_EMAIL_H
