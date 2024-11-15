//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/System/Blackboard.h"
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

void test_blackboard(){
    std::cout << "=====Testing BlackBoard=====" << std::endl;
    std::cout << "---Testing Login---" << std::endl;
    //basic info input
    std::string username = "";
    std::string password = "";
    std::cout << "Username:" << std::endl;
    std::cin >> username;
    std::cout << "Password:" << std::endl;
    std::cin >> password;
    auto *bb = new BlackBoardSystem(username, password);
    if(bb->login()){

        std::cout << "---Testing show_command---" << std::endl;
        std::cout << bb->get_commands() << std::endl;

        std::cout << "---Testing get_course---" << std::endl;
        std::cout << bb->get_course() << std::endl;

        std::cout << "---Testing get_announcement---" << std::endl;
        std::cout << bb->get_announcement("CSC3002") << std::endl;

        std::cout << "---Testing get_undo_assignments---" << std::endl;
        std::cout << bb->get_undo_assignment() << std::endl;
    }
    delete bb;
}

int main() {
    //Test Libcurl
    test_libcurl();

    //Test Libxml
    test_libxml();

    //Test bb class
    test_blackboard();

    return 0;
}
