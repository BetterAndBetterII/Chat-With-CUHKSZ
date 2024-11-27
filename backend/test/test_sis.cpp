#include <iostream>
#include "../include/System/Sis.h"
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>


void test_sis(){
    std::cout << "=====Testing Sis=====" << std::endl;
    std::cout << "---Testing Login---" << std::endl;
    //basic info input
    std::string username = "";
    std::string password = "";
    std::cout << "Username:" << std::endl;
    std::cin >> username;
    std::cout << "Password:" << std::endl;
    std::cin >> password;
    auto *sis = new SisSystem(username, password);
    if(sis->login()){

        /* std::cout << "---Testing show_command---" << std::endl;
        std::cout << sis->get_commands() << std::endl;

        std::cout << "---Testing get_course---" << std::endl;
        std::cout << sis->get_course() << std::endl;

        std::string crouse;
        std::cout << "---Testing get_announcement---" << std::endl;
        std::cout << "Crouse Name?(e.g. CSC3001)" <<std::endl;
        std::cin >> crouse;
        std::cout << sis->get_announcement(crouse) << std::endl;

        std::cout << "---Testing get_assignments---" << std::endl;
        std::cout << "Crouse Name?(e.g. CSC3001)" <<std::endl;
        std::cin >> crouse;
        std::cout << sis->get_assignment(crouse) << std::endl;   

        std::cout << "---Testing get_grades---" << std::endl;
        std::cout << "Crouse Name?(e.g. CSC3001)" <<std::endl;
        std::cin >> crouse;
        std::cout << sis->get_grades(crouse) << std::endl; */
    }
    delete sis;
}

int main() {
    //Test sis class
    test_sis();

    return 0;
}
