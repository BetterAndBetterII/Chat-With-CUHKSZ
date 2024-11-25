
#ifndef CHAT_WITH_CUHKSZ_LIBARY_H
#define CHAT_WITH_CUHKSZ_LIBARY_H

#include "../../include/System/CurlGlobal.h"
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h> 
#include <openssl/ssl.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <set>
#include <vector>

class LibarySystem{
private:
    std::string username;
    std::string password;

    std::set<std::string> command_list;
    
    bool is_login;

    // 定义信息结构体
    struct Info {
        std::string title;
        std::string publisher;
        std::string type;
        std::optional<std::string> creator; // 可选字段
        std::optional<std::string> subject; // 可选字段
    };

    //cookiefile存储libcurl生成的cookie文件的路径
    std::string cookiefile;

    CURL* libary_handle;
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
    static CURLcode ssl_ctx_callback(CURL *curl, void *ssl_ctx, void *userptr );

    //command helper functions
    std::string getRequest(const std::string& url)const;

    std::string postRequest(const std::string& url, const std::string& strdata)const;

    std::vector<std::string> xpathQuery(const std::string& xmlContent, const std::string& xpathExpr)const;

    std::string vector_toString(const std::vector<std::string>& vector)const;
    
    //删除字符串前后多余空格和换行符
    std::string space_cutter(const std::string& str)const;

public:
    // 构造函数
    LibarySystem(const std::string& username, const std::string& password);

    //析构函数
    ~LibarySystem();

    //返回command_list
    std::string get_commands() const;

    //登录
    bool login();

    //初始化时若输入了错误的密码可以用change_info()来修改
    bool change_info(const std::string& username, const std::string& password);
    //若未登录成功(is_login == false)，则修改账号密码并返回true，若已登录则不会修改账号密码，返回false

    std::string search(const std::string& keyword)const;
};


#endif //CHAT_WITH_CUHKSZ_LIBARY_H