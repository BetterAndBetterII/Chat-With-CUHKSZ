//
// Created by Gary on 24-11-24.

#ifndef CHAT_WITH_CUHKSZ_BOOKING_H
#define CHAT_WITH_CUHKSZ_BOOKING_H

#include "../../include/System/System.h"
#include <map>
#include <nlohmann/json.hpp>
#include <iostream> //std::cerr

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

class BookingSystem : public System
{
private:

    //field_dict用于存储field_name和field_id的映射
    std::map<std::string, std::string> field_dict = {{"badminton", "1097"}};

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
    // 构造函数
    BookingSystem(const std::string& username, const std::string& password);

    //析构函数
    ~BookingSystem();

    //登录
    bool login();

    //booker类
    Booker booker;

    std::string set_booker(const std::string& telephone, const std::string& reason, const std::string& details);

    [[nodiscard]] std::string get_available_time(const std::string& field_name, const std::string& date_string);

    [[nodiscard]] std::string book_place(const std::string& field_name, const std::string& place_id, const std::string& start_time, const std::string& end_time);
    std::string book_field(const std::string& field_name, const std::string& start_time, const std::string& end_time);
};


#endif //CHAT_WITH_CUHKSZ_BOOKING_H
