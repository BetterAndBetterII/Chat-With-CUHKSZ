#include <stdexcept>
#include <curl/curl.h>
#include "../../include/System/CurlGlobal.h"
#include <iostream>

CurlGlobal::CurlGlobal() {
    std::lock_guard<std::mutex> lock(init_mutex);
    if (init_count == 0) {
        if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
            throw std::runtime_error("Failed to initialize libcurl");
        }
        else{
            //调试用，检测curlglobal是否正确initialize
            //std::cout << "curl globally initialized" << std::endl;
        }
    }
    ++init_count;
}

CurlGlobal::~CurlGlobal() {
    std::lock_guard<std::mutex> lock(init_mutex);
    if (--init_count == 0) {
        curl_global_cleanup();
        //调试用，检测curlglobal是否正确clean up
        //std::cout << "curl globally cleaned" << std::endl;
    }
}
