#include <iostream>
#include "../include/System/Sis.h"
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <cstdlib>


void test_sis(){
    std::cout << "=====Testing Sis=====" << std::endl;
    std::cout << "---Testing Login---" << std::endl;
    //basic info input
    std::string username = "";
    std::string password = "";
    const char* env_username = std::getenv("STUDENT_USERNAME");
    const char* env_password = std::getenv("STUDENT_PASSWORD");
    if (env_username) {
        username = env_username;
    } else {
        std::cout << "Username:" << std::endl;
        std::getline(std::cin, username);
        std::cin >> username;
    }
    if (env_password) {
        password = env_password;
    } else {
        std::cout << "Password:" << std::endl;
        std::getline(std::cin, password);
    }
    auto *sis = new SisSystem(username, password);
    if(sis->login()){

        std::cout << "---Testing get_command---" << std::endl;
        std::cout << sis->get_commands() << std::endl;

        std::cout << "---Testing get_schedule---" << std::endl;
        std::cout << sis->get_schedule() << std::endl;

        std::cout << "---Testing get_course---" << std::endl;
        std::string course;
        std::cout << "Course Name?(e.g. CSC3001)Press'exit'to stop" <<std::endl;
        std::getline(std::cin, course);
        while (course!="exit")
        {
            std::cout << sis->get_course(course) << std::endl;
            std::cout << "Course Name?(e.g. CSC3001)Press'exit'to stop" <<std::endl;
            std::getline(std::cin, course);
        }

        std::cout << "---Testing get_grades---" << std::endl;
        std::cout << "Term?(e.g.2023-24 Term 2)" <<std::endl;
        std::string term;
        std::getline(std::cin, term);
        std::cout << sis->get_grades(term) << std::endl; 
    }
    delete sis;
}

int main() {
    //Test sis class
    test_sis();

    return 0;
}
