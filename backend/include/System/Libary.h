
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

    std::set<std::string> command_list;
    
    // 定义信息结构体
    struct Info {
        std::optional<std::string> title;
        std::optional<std::string> publisher;
        std::optional<std::string> type;
        std::optional<std::string> creator; // 可选字段
        std::optional<std::string> subject; // 可选字段
    };

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

    static const int DEAFULT_SEARCH_NUMBER = 3;

    // 构造函数
    LibarySystem();

    //析构函数
    ~LibarySystem();

    //返回command_list
    std::string get_commands() const;

    std::string search(const std::string& keyword, const int& limit = DEAFULT_SEARCH_NUMBER, const std::string& tab="Everything")const;
    /*参数：
     *keyword: 查询内容
     *limit: 返回的数量，默认为3
     *tab: 查询的范围
     *可输入的值："Everything", "PrintBooks/Journals", "Articles/eBooks"
    */
};


#endif //CHAT_WITH_CUHKSZ_LIBARY_H