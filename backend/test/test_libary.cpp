#include <iostream>
#include "../include/System/Libary.h"
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

void test_libary(){
    std::cout << "=====Testing Libary=====" << std::endl;
    std::cout << "---Testing Login---" << std::endl;
    //basic info input
    std::string username = "";
    std::string password = "";
    std::cout << "Username:" << std::endl;
    std::cin >> username;
    std::cout << "Password:" << std::endl;
    std::cin >> password;
    auto *libary = new LibarySystem(username, password);
    if(libary->login()){
        std::string keyword;
        std::cout << "Keyword?" << std::endl;
        std::cin.ignore();//跳过换行符
        std::getline(std::cin, keyword) ;
        std::cout << libary->search(keyword) << std::endl;
    }
    delete libary;
}

int main() {

    //Test libary class
    test_libary();

    return 0;
}