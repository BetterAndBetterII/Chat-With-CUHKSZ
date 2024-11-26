#include <iostream>
#include "../include/System/Libary.h"
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

void test_libary(){
    std::cout << "=====Testing Libary=====" << std::endl;
    auto *libary = new LibarySystem();
    std::cout << "---Testing getCommand---" << std::endl;
    std::cout << libary->get_commands() << std::endl;
    std::cout << "---Testing Search---" << std::endl;
    std::string keyword;
    std::string limit;
    std::string tab;
    std::cout << "Keyword?" << std::endl;
    std::getline(std::cin, keyword) ;
    std::cout << "limit?" << std::endl;
    std::getline(std::cin, limit);
    std::cout << "tab? (Everything, PrintBooks/Journals, Articles/eBooks)" << std::endl;
    std::getline(std::cin, tab) ;
    std::cout << libary->search(keyword, std::stoi(limit), tab) << std::endl;
    delete libary;
}

int main() {

    //Test libary class
    test_libary();

    return 0;
}