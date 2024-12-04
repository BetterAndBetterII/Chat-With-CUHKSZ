#include <cstring>
#include <iostream>
#include <curl/curl.h>

int main() {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // 进行一个简单的请求，确保协议信息能被填充
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");

        // 执行请求以便填充协议信息
        res = curl_easy_perform(curl);

        if(res == CURLE_OK) {
            // 成功后获取协议信息
            char *scheme;
            res = curl_easy_getinfo(curl, CURLINFO_SCHEME, &scheme);

            if (res == CURLE_OK && scheme) {
                std::cout << "Supported scheme: " << scheme << std::endl;

                // 检查是否支持SMTP协议
                if (strstr(scheme, "smtp")) {
                    std::cout << "SMTP protocol is supported." << std::endl;
                } else {
                    std::cerr << "SMTP protocol is NOT supported." << std::endl;
                }

                // 检查是否支持SSL/TLS
                if (strstr(scheme, "https") || strstr(scheme, "ftps")) {
                    std::cout << "SSL/TLS support is available." << std::endl;
                } else {
                    std::cerr << "SSL/TLS support is NOT available." << std::endl;
                }
            } else {
                std::cerr << "Failed to retrieve scheme info: " << curl_easy_strerror(res) << std::endl;
            }
        } else {
            std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}
