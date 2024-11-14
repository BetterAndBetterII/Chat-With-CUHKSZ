//
// Created by yf on 11/15/24.
//
#include "../include/Knowledge/KnowledgeBase.h"
#include <iostream>

int main() {
    KnowledgeBase KnowledgeBase;
    std::string filePath = "../KnowledgeBase/phoenix_cuhksz_knowledge-main";
    KnowledgeBase.loadAllFiles(filePath);
    std::string question;
    std::cin >> question;
    while (question != "stop") {
        std::cin >> question;
        std::string response = KnowledgeBase.searchKnowledgeBase(KnowledgeBase.getRecords(), question);
        std::cout << response << std::endl;
    }
}