//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/System/Email.h"
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

    // 检查是否支持 SMTP
    if(info->features & CURLPROTO_SMTPS) {
        std::cout << "支持 SMTP: 是" << std::endl;
    } else {
        std::cout << "支持 SMTP: 否" << std::endl;
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

void test_email(){
    std::cout << "=====Testing Email=====" << std::endl;
    std::cout << "---Testing Login---" << std::endl;
    //basic info input
    std::string username;
    std::string password;
    std::cout << "Username:" << std::endl;
    std::cin >> username;
    std::cout << "Password:" << std::endl;
    std::cin >> password;
    auto *email = new EmailSystem(username, password);
    std::string html_body = "<h1>标题</h1><h2>This is a test email from Chat-With-CUHKSZ.</h2><p>正文的样式</p>";
    if(email->login()){
        std::cout << email->send_email({username + "@link.cuhk.edu.cn"}, "Test", html_body) << std::endl;
    }
    delete email;
}

int main() {
    //Test Libcurl
    test_libcurl();

    //Test Libxml
    test_libxml();

    //Test email class
    test_email();

    return 0;
}
