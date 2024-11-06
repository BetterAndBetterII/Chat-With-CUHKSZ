//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/System/Blackboard.h"

//no need to #define CPPHTTPLIB_OPENSSL_SUPPORT
//already defined in cmakelist 
#include "httplib.h"
#include <fstream>

//用于删除“set-cookie”尾部键值对
std::string cutTail( const std::string& undealtCookie ){
    int end = 0 ;
    while(undealtCookie[end] != ';'){
        end++;
    }
    return undealtCookie.substr(0, end);
}

int main() {
//    std::cout << "Testing BlackBoard~" << std::endl;
//    std::string username = "Gary";
//    std::string password = "12345678";
//    auto *bb = new BlackBoardSystem(username, password);
//    std::string command = "command0";
//    std::cout << bb->execute_command(command) << std::endl;
//    command = "command2";
//    std::cout << bb->execute_command(command) << std::endl;
//    command = "command1";
//    std::cout << bb->execute_command(command) << std::endl;
//
//    std::cout << "Testing httplib.h" << std::endl;
//     httplib::SSLClient cli("sse.cuhk.edu.cn");
//    // Get request
//    if (auto res = cli.Get("/faculty/huangrui") ) {
//        std::cout << "Status code =" << res->status << std::endl;
//        if( res->status == 200){
//            std::cout << "Response headers:\n";
//            // print headers
//            for (const auto& header : res->headers) {
//            std::cout << header.first << ": " << header.second << "\n";
//            }
//     	    std::cout << "woohooo! page  down to current directory!" << std::endl;
//            std::ofstream outfile("profHuang.html");
//            outfile << res->body << std::endl;
//        }
//        else {
//            std::cout << "stutus code != 200 , status code = " << res->status << std::endl;
//        }
//    }
//    else {
//            auto err = res.error();
//            std::cout << httplib::to_string(err) << std::endl;
//    }
//
    std::cout << "Test Login BB" << std::endl;
    std::string bbusername;
    std::string bbpassword;
    std::cout << "Type username:" << std::endl;
    std::cin >> bbusername;
    std::cout << "Type password:" << std::endl;
    std::cin >> bbpassword;
//    httplib::SSLClient bbcli("sts.cuhk.edu.cn");
//    bbcli.set_follow_location(true);
//    std::string url = "/adfs/oauth2/authorize?response_type=code&client_id=4b71b947-7b0d-4611-b47e-0ec37aabfd5e&redirect_uri=https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode&client-request-id=53894b74-3b79-409d-cf05-0240030000cf";
//
//    httplib::Headers headers = {
//        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36" },
//        { "Connection", "close"}
//    };
//    httplib::Params params{
//        {"UserName", "cuhksz\\"+bbusername},
//        {"Password", bbpassword},
//        {"Kmsi", "true"},
//        {"AuthMethod", "FormsAuthentication"}
//    };
//
//    if(auto bbres = bbcli.Post( url, headers, params )){
//	    std::cout << "Response code:" << bbres->status << std::endl;
//	    std::cout << "-----The below is the body----" << std::endl;
//	    std::cout << bbres->body << std::endl;
//    }
//    else{
//        auto bberr = bbres.error();
//        std::cout << "Post request failed because :" << std::endl;
//        std::cout << httplib::to_string(bberr) << std::endl;
//    }
//    httplib::SSLClient bbcli("sts.cuhk.edu.cn");
//    bbcli.set_follow_location(false); // 手动跟踪重定向，便于记录每一步
//    std::string url = "/adfs/oauth2/authorize?response_type=code&client_id=4b71b947-7b0d-4611-b47e-0ec37aabfd5e&redirect_uri=https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode&client-request-id=53894b74-3b79-409d-cf05-0240030000cf";
//
//    httplib::Headers headers = {
//        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36" },
//        { "Connection", "close" }
//    };
//
//    httplib::Params params = {
//        { "UserName", "cuhksz\\" + bbusername },
//        { "Password", bbpassword },
//        { "Kmsi", "true" },
//        { "AuthMethod", "FormsAuthentication" }
//    };
//
//    for (int i = 0; i < 3; ++i) { // 最多跟踪3次重定向
//        if (auto bbres = bbcli.Post(url, headers, params)) {
//            std::cout << "Response code: " << bbres->status << std::endl;
//            
//            if (bbres->status >= 300 && bbres->status < 400) { // 检查是否为重定向状态码
//                auto location = bbres->get_header_value("Location");
//                std::cout << "Response headers:\n";
//                // print headers
//                for (const auto& header : bbres->headers) {
//                std::cout << header.first << ": " << header.second << "\n";
//                }
//                std::cout << "-----------------------" << location << std::endl;
//                std::cout << "Redirected to: " << location << std::endl;
//
//                if (!location.empty()) {
//                    // 更新 URL 进行下一次请求
//                    url = location;
//                } else {
//                    std::cout << "No further Location header found." << std::endl;
//                    break;
//                }
//            } else {
//                std::cout << "Final response body:\n" << bbres->body << std::endl;
//                break;
//            }
//        } else {
//            auto bberr = bbres.error();
//            std::cout << "Post request failed because: " << httplib::to_string(bberr) << std::endl;
//            break;
//        }
//    }
    httplib::SSLClient bbcli("sts.cuhk.edu.cn");
    bbcli.set_follow_location(false); // 手动跟随重定向

    std::string initial_url = "/adfs/oauth2/authorize?response_type=code&client_id=4b71b947-7b0d-4611-b47e-0ec37aabfd5e&redirect_uri=https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode&client-request-id=53894b74-3b79-409d-cf05-0240030000cf";
    httplib::Headers headers = {
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36" },
    };

    httplib::Params params = {
        { "UserName", "cuhksz\\" + bbusername },
        { "Password", bbpassword },
        { "Kmsi", "true" },
        { "AuthMethod", "FormsAuthentication" }
    };

    // 初次请求，获取 cookie 和重定向位置
    if (auto bbres = bbcli.Post(initial_url, headers, params)) {
        if (bbres->status >= 300 && bbres->status < 400) {

            /* //print responce code and headers
            std::cout << "Response code:" << bbres->status << std::endl;
            std::cout << "Response headers:\n";
            for (const auto& header : bbres->headers) {
            std::cout << header.first << ": " << header.second << "\n";
            } */

            // 处理Set-Cookie
            std::string cookie = "" ;
            bool isFirst = true;
            for (const auto& header : bbres->headers) {
                if(header.first == "Set-Cookie" && isFirst){
                    isFirst = false;
                    cookie += cutTail(header.second);
                }
                else if(header.first == "Set-Cookie"){
                    cookie += "; " + cutTail(header.second);
                }             
            }

            // 将 Cookie 加入下一次请求的 headers 中
            headers.emplace("Cookie", cookie);

            //获取重定向地址
            auto redirect_url = bbres->get_header_value("Location");

            //print redirect_url
            //std::cout << "Redirecting to: " << redirect_url << std::endl;


            // 发出重定向后的请求
            bbres = bbcli.Get(redirect_url.c_str(), headers);
            if (bbres) {
                /* std::cout << "Second response code: " << bbres->status << std::endl;
                std::cout << "Response headers:\n";
                // print headers
                for (const auto& header : bbres->headers) {
                    std::cout << header.first << ": " << header.second << "\n";
                }
                std::cout << "Second response body:\n" << bbres->body << std::endl; */
                
                //第二次重定向
                // 处理Set-Cookie std::string cookie = "" ;
                bool isFirst = true;
                for (const auto& header : bbres->headers) {
                    if(header.first == "Set-Cookie" && isFirst){
                        isFirst = false;
                        cookie += cutTail(header.second);
                    }
                    else if(header.first == "Set-Cookie"){
                        cookie += "; " + cutTail(header.second);
                    }             
                }

                // 处理Set-Cookie
                cookie = "" ;
                isFirst = true;
                for (const auto& header : bbres->headers) {
                    if(header.first == "Set-Cookie" && isFirst){
                        isFirst = false;
                        cookie += cutTail(header.second);
                    }
                    else if(header.first == "Set-Cookie"){
                        cookie += "; " + cutTail(header.second);
                    }             
                }

                // 将 Cookie 加入下一次请求的 headers 中
                headers.erase("Cookies");
                headers.emplace("Cookie", cookie);
                headers.emplace("Host", "bb.cuhk.edu.cn");

                //获取重定向地址
                redirect_url = bbres->get_header_value("Location");

                //print redirect_url
                //std::cout << "Redirecting to: " << redirect_url << std::endl;


                // 发出重定向后的请求
                bbres = bbcli.Get(redirect_url.c_str(), headers);
                if (bbres) {
                    std::cout << "Third response code: " << bbres->status << std::endl;
                    std::cout << "Response headers:\n";
                    // print headers
                    for (const auto& header : bbres->headers) {
                        std::cout << header.first << ": " << header.second << "\n";
                    }
                    std::cout << "Third response body:\n" << bbres->body << std::endl;

                }
                else {
                auto bberr = bbres.error();
                std::cout << "Third request failed because: " << httplib::to_string(bberr) << std::endl;
                }
            }
            else {
                auto bberr = bbres.error();
                std::cout << "Second request failed because: " << httplib::to_string(bberr) << std::endl;
            }
        }
    } 
    else {
        auto bberr = bbres.error();
        std::cout << "Initial request failed because: " << httplib::to_string(bberr) << std::endl;
    }

    return 0;
}
