#include <iostream>
#include <curl/curl.h>

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