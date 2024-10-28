//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/System/Blackboard.h"

//no need to #define CPPHTTPLIB_OPENSSL_SUPPORT
//already defined in cmakelist 
#include "httplib.h"

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
//     httplib::SSLClient cli("google.com");
//     cli.enable_server_certificate_verification(false);
//
//    // Get request
//    if (auto res = cli.Get("/") ) {
//        std::cout << "Status code =" << res->status << std::endl;
//
//        //redirect
//        cli.set_follow_location(true);
//        std::cout << "Status code =" << res->status << std::endl;
//
//        res = cli.Get("/");
//        std::cout << "Status code =" << res->status << std::endl;
//        if( res->status == 200){
//            std::cout << "Response headers:\n";
//            // print headers
//            for (const auto& header : res->headers) {
//            std::cout << header.first << ": " << header.second << "\n";
//            }
//        }
//        else {
//            std::cout << "stutus code != 200 , status code = " << res->status << std::endl;
//        }
//    }
//    else {
//            auto err = res.error();
//            std::cout << httplib::to_string(err) << std::endl;
//    }

    std::cout << "Test Login BB" << std::endl;
    std::string username;
    std::string password;
    std::cout << "Type username:" << std::endl;
    std::cin >> username;
    std::cout << "Type password:" << std::endl;
    std::cin >> password;
    httplib::SSLClient bbcli("sts.cuhk.edu.cn");
    bbcli.set_follow_location(true);
    std::string url = "/adfs/oauth2/authorize?response_type=code&client_id=4b71b947-7b0d-4611-b47e-0ec37aabfd5e&redirect_uri=https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode&client-request-id=53894b74-3b79-409d-cf05-0240030000cf";

    httplib::Headers headers = {
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36" },
        { "Connection", "close"}
    };
    httplib::Params params{
        {"UserName", "cuhksz\\"+username},
        {"Password", password},
        {"Kmsi", "true"},
        {"AuthMethod", "FormsAuthentication"}
    };
    
    if(auto bbres = bbcli.Post( url, headers, params )){
	    std::cout << "Response code:" << bbres->status << std::endl;
	    std::cout << "-----The below is the body----" << std::endl;
	    std::cout << bbres->body << std::endl;
    }
    else{
        auto bberr = bbres.error();
        std::cout << "Post request failed because :" << std::endl;
        std::cout << httplib::to_string(bberr) << std::endl;
    }

    return 0;
}
