//
// Created by Gary on 24-10-24.
//
#include <iostream>
#include "../include/System/Blackboard.h"

//no need to #define CPPHTTPLIB_OPENSSL_SUPPORT
//already defined in cmakelist 
#include <httplib.h>

int main() {
    std::cout << "Testing BlackBoard~" << std::endl;
    std::string username = "Gary";
    std::string password = "12345678";
    auto *bb = new BlackBoardSystem(username, password);
    std::string command = "command0";
    std::cout << bb->execute_command(command) << std::endl;
    command = "command2";
    std::cout << bb->execute_command(command) << std::endl;
    command = "command1";
    std::cout << bb->execute_command(command) << std::endl;

    std::cout << "Testing httplib.h" << std::endl;
     httplib::SSLClient cli("google.com");
     cli.enable_server_certificate_verification(false);

    // Get request
    if (auto res = cli.Get("/") ) {
        std::cout << "Status code =" << res->status << std::endl;

        //redirect
        cli.set_follow_location(true);
        std::cout << "Status code =" << res->status << std::endl;

        res = cli.Get("/");
        if( res->status == 200){
            std::cout << "Response headers:\n";
            // print headers
            for (const auto& header : res->headers) {
            std::cout << header.first << ": " << header.second << "\n";
            }
        }
        else {
            std::cout << "stutus code != 200 , status code = " << res->status << std::endl;
        }
    }
    else {
            auto err = res.error();
            std::cout << httplib::to_string(err) << std::endl;
    }

    return 0;
}
