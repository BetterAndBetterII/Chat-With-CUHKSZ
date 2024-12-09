#ifndef CHAT_WITH_CUHKSZ_LIBARY_H
#define CHAT_WITH_CUHKSZ_LIBARY_H

#include "../../include/System/System.h"
#include <openssl/ssl.h>
#include <nlohmann/json.hpp>
#include <optional>

class LibarySystem : public System{
private:
    
    // 定义信息结构体
    struct Info {
        std::optional<std::string> title;
        std::optional<std::string> publisher;
        std::optional<std::string> type;
        std::optional<std::string> creator; 
        std::optional<std::string> subject; 
    };

    //处理接受json数据时产生的unsafe legacy problem
    static CURLcode ssl_ctx_callback(CURL *curl, void *ssl_ctx, void *userptr );

public:

    static const int DEAFULT_SEARCH_NUMBER = 3;

    //TODO
    bool login();

    // 构造函数
    LibarySystem(const std::string& username, const std::string& password);

    //析构函数
    ~LibarySystem();

    std::string search(const std::string& keyword, const int& limit = DEAFULT_SEARCH_NUMBER, const std::string& tab="Everything");
    /*参数：
     *keyword: 查询内容
     *limit: 返回的数量，默认为3
     *tab: 查询的范围
     *可输入的值："Everything", "PrintBooks/Journals", "Articles/eBooks"
    */
};


#endif //CHAT_WITH_CUHKSZ_LIBARY_H