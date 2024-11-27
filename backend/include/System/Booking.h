//
// Created by Gary on 24-11-24.

#ifndef CHAT_WITH_CUHKSZ_BLACKBOARD_H
#define CHAT_WITH_CUHKSZ_BLACKBOARD_H

#include <map>

#include "../../include/System/CurlGlobal.h"
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <set>
#include <vector>
#include <nlohmann/json.hpp>
#include <iostream> //std::cerr
#include <sstream>
#include <cstring>
#include <regex> //space_cutter()
#include <filesystem> //destructor
#include <utility>
#include <chrono>
#include <memory>

namespace MESSAGE {
    const std::string SUCCESS = "预订成功";
    const std::string LOGIN_REQUIRED = "请先登录";
    const std::string BOOKER_INFO_REQUIRED = "请先设置预订人信息";
}

class BookEvent
{
public:
    std::string startTime;
    std::string endTime;
    std::string placeID;
    std::string booker;
    std::string reason;
};

class FieldEvent
{
public:
    std::string field_id;
    std::string field_name;
    std::vector<BookEvent> book_info;
    std::map<std::string, std::string> places;

    FieldEvent(
        std::string field_id,
        std::string field_name,
        const std::vector<BookEvent>& book_info,
        const std::map<std::string, std::string>& places
    );
    [[nodiscard]] std::vector<std::string> is_available(const std::string& start_time,
                                                        const std::string& end_time) const;

private:
    [[nodiscard]] bool is_available(const std::string& start_time, const std::string& end_time,
                                    const std::vector<BookEvent>& book_info) const;
};

class Booker
{
public:
    Booker(
        std::string  telephone,
        std::string  reason,
        std::string  details
    );

    std::string telephone;
    std::string reason;
    std::string details;
};

class BookingSystem
{
private:
    std::string username;
    std::string password;

    std::set<std::string> command_list;

    bool is_login;

    //cookiefile存储libcurl生成的cookie文件的路径
    std::string cookiefile;

    //curlglobal类用于避免多次全局初始化
    CurlGlobal curl_global_manager;

    //Memory结构体用于存储请求返回的数据
    struct Memory
    {
        char* response;
        size_t size;
    };

    //field_dict用于存储field_name和field_id的映射
    std::map<std::string, std::string> field_dict = {{"badminton", "1097"}};

    //callback functions用于处理返回内容
    static size_t ignore_calback(void* ptr, size_t size, size_t nmemb, void* userdata);
    static size_t write_callback(char* data, size_t size, size_t nmemb, void* clientp);

    //command helper functions
    [[nodiscard]] std::string getRequest(const std::string& url) const;

    [[nodiscard]] std::string postRequest(const std::string& url, const std::string& strdata) const;

    [[nodiscard]] std::vector<std::string>
    xpathQuery(const std::string& xmlContent, const std::string& xpathExpr) const;

    //删除字符串前后多余空格和换行符
    [[nodiscard]] std::string space_cutter(const std::string& str) const;

    [[nodiscard]] static std::vector<BookEvent> _parse_field_info(const std::string& raw_data);

    [[nodiscard]] std::map<std::string, std::string> get_field_places(std::string field_id) const;

    [[nodiscard]] static std::map<std::string, std::string> _parse_field_places(const std::string& raw_data);

    [[nodiscard]] std::map<std::string, std::string> _get_info(
        const std::string& start_time,
        const std::string& end_time,
        const std::string& placeID
    ) const;

    [[nodiscard]] static std::map<std::string, std::string> _parse_info(const std::string& data) ;

    //接受field_name, start_time, end_time作为参数，返回field_name, start_time, end_time组成的字符串
    [[nodiscard]] FieldEvent retrieve_field_info(const std::string& field, const std::string& start_time,
                                                 const std::string& end_time);

    [[nodiscard]] FieldEvent retrieve_field_info_day(const std::string& field, const std::string& date_string);

    [[nodiscard]] std::string book(
        const FieldEvent& field_event,
        const std::string& placeID,
        const std::string& start_time,
        const std::string& end_time
    ) const;

public:
    CURL* booking_handle;
    // 构造函数
    BookingSystem(const std::string& username, const std::string& password);

    //析构函数
    ~BookingSystem();

    //返回command_list
    [[nodiscard]] std::string get_commands() const;

    //登录
    bool login();

    //初始化时若输入了错误的密码可以用change_info()来修改
    bool change_info(const std::string& username, const std::string& password);
    //若未登录成功(is_login == false)，则修改账号密码并返回true，若已登录则不会修改账号密码，返回false

    //booker类
    Booker booker;

    void set_booker(const std::string& telephone, const std::string& reason, const std::string& details);

    [[nodiscard]] std::string get_available_time(const std::string& field_name, const std::string& date_string);

    [[nodiscard]] std::string book_place(const std::string& field_name, const std::string& place_id, const std::string& start_time, const std::string& end_time);
    std::string book_field(const std::string& field_name, const std::string& start_time, const std::string& end_time);
};


#endif //CHAT_WITH_CUHKSZ_BLACKBOARD_H
