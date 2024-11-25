//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/System/Booking.h"
//Test libcurl
#include <curl/curl.h>
//Test libxml
#include <libxml/parser.h>
#include <libxml/xpath.h>

void test_libxml() {
    LIBXML_TEST_VERSION
    std::cout << "=====Test libxml=====" << std::endl;
    std::cout << "libxml2成功链接" << std::endl;
    xmlCleanupParser(); // 清理libxml2的全局状态
}

void test_libcurl(){
    std::cout << "=====Test libcurl=====" << std::endl;

    //检查libcurl版本信息
    curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);
    std::cout << "libcurl 版本: " << info->version << std::endl;

    // 检查是否支持 SSL
    if(info->features & CURL_VERSION_SSL) {
        std::cout << "支持 SSL: 是" << std::endl;
    } else {
        std::cout << "支持 SSL: 否" << std::endl;
    }

    // 检查是否支持 HTTP/2
    if(info->features & CURL_VERSION_HTTP2) {
        std::cout << "支持 HTTP/2: 是" << std::endl;
    } else {
        std::cout << "支持 HTTP/2: 否" << std::endl;
    }

    // 检查是否支持 HTTP3
    if(info->features & CURL_VERSION_HTTP3) {
        std::cout << "支持 HTTP/3: 是" << std::endl;
    } else {
        std::cout << "支持 HTTP/3: 否" << std::endl;
    }

}

void test_booking(){
    std::cout << "=====Testing Booking=====" << std::endl;
    std::cout << "---Testing Login---" << std::endl;
    //basic info input
    std::string username;
    std::string password;
    std::cout << "Username:" << std::endl;
    std::cin >> username;
    std::cout << "Password:" << std::endl;
    std::cin >> password;
    std::string telephone = "1324567890";
    std::string reason = "test";
    std::string details = "test";
    std::string field_name = "badminton";
    std::string start_time = "2024-11-25 18:00";
    std::string end_time = "2024-11-25 19:00";
    auto *booking = new BookingSystem(username, password);
    if(booking->login()){
        std::cout << "---Testing retrieve_field_info---" << std::endl;
        std::string available_time = booking->get_available_time("badminton", "2024-11-25");
        std::cout << available_time << std::endl;

        booking->set_booker(telephone, reason, details);
        std::string res = booking->book_field(field_name, start_time, end_time);
        std::cout << res << std::endl;
    }
    delete booking;
}

int main() {
    //Test Libcurl
    test_libcurl();

    //Test Libxml
    test_libxml();

    //Test booking class
    test_booking();

    return 0;
}
