#ifndef CHAT_WITH_CUHKSZ_SYSTEM_H
#define CHAT_WITH_CUHKSZ_SYSTEM_H

#include "../../include/System/CurlGlobal.h"
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h> 
#include <set>
#include <vector>

class System{
protected:
    std::string username;
    std::string password;

    
    bool is_login;

    //cookiefile存储libcurl生成的cookie文件的路径
    std::string cookiefile;

    CURL* handle;
    struct curl_slist* headers;
    //curlglobal类用于避免多次全局初始化
    CurlGlobal curl_global_manager;

    //Memory结构体用于存储请求返回的数据
    struct Memory {
    char* response;
    size_t size;
    };

    //callback functions用于处理返回内容
    static size_t ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t write_callback(char *data, size_t size, size_t nmemb, void *clientp);
    static size_t header_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr);

    //command helper functions
    std::string getRequest(const std::string& url)const;

    std::string postRequest(const std::string& url, const std::string& strdata)const;

    std::vector<std::string> xpathQuery(const std::string& xmlContent, const std::string& xpathExpr)const;

public:

    
    // 构造函数
    System(const std::string& username, const std::string& password);

    //析构函数
    ~System();

    //返回command_list
    virtual std::string get_commands() const=0;

    //登录
    virtual bool login()=0;

    //初始化时若输入了错误的密码可以用change_info()来修改
    bool change_info(const std::string& username, const std::string& password);
    //若未登录成功(is_login == false)，则修改账号密码并返回true，若已登录则不会修改账号密码，返回false

    
};


#endif //CHAT_WITH_CUHKSZ_SYSTEM_H
