//
// Created by yf on 11/15/24.
//
#include "../include/Knowledge/KnowledgeBase.h"
#include <iostream>

int main() {
    KnowledgeBase KnowledgeBase;
    std::string filePath = "../KnowledgeBase/phoenix_cuhksz_knowledge-main";
    KnowledgeBase.loadAllFiles(filePath);
    std::string keyword1,keyword2, keyword3;
    while (true) {
        std::cin >> keyword1>>keyword2>>keyword3;
        std::string response = KnowledgeBase.searchKnowledgeBase(KnowledgeBase.getRecords(),
                                                                        keyword1, 20,
                                                                        keyword2, 5,
                                                                        keyword3, 1);
        std::cout << response << std::endl;
    }
}