// Created by Gary on 24-10-23.
#include "../include/Knowledge/KnowledgeBase.h"
#include <iostream>
#include <fstream>

//通过KnowledgeBase对象加载知识库
void KnowledgeBase::loadAllFiles(const std::string& basePath) {

    for (const auto& entry : std::filesystem::recursive_directory_iterator(basePath)) {//遍历主文件夹下的所有文件路径
        if (entry.path().extension() == ".md") {//如果文件为.md文件
            std::vector<KnowledgeRecord> records = this->parseFiles(entry.path(), entry.path().filename().string());//录入文件，并储存到vector容器中
            this->records.insert(this->records.end(), records.begin(), records.end());//把该vector容器归并到records知识库里
        }
    }
}

//获取KnowledgeBase对象中加载的records知识库
std::vector<KnowledgeRecord> KnowledgeBase::getRecords() const {
    return this->records;
}

//录入文件内容
std::vector<KnowledgeRecord> KnowledgeBase::parseFiles(const std::filesystem::path& directoryPath, const std::string& title) {
    std::vector<KnowledgeRecord> records;
    std::string path=directoryPath.string();
    std::cout << "Processing file: " << path << std::endl;

    std::ifstream file(directoryPath);//将wifsream连接到指定路径的文件
    
    if (file.is_open()) {
        KnowledgeRecord record;
        record.title = title;//录入文件名

        std::string line;
        while (std::getline(file, line)) {
            record.content += line + "\n"; 
        }//录入文件内容

        records.push_back(record);//将文件内容储存到一个vector容器里

        file.close();
    } else {
        std::cerr << "Unable to open file: " << directoryPath << std::endl;
    }

    return records;
}

//检索知识库中的关键词
std::string KnowledgeBase::searchKnowledgeBase(const std::vector<KnowledgeRecord>& records, const std::string& keyword) {
    for (const auto& record : records) {
        if (record.title.find(keyword) != std::string::npos || record.content.find(keyword) != std::string::npos) {
            return record.content;
        }
    }
    return "No matching record found.";
}

//处理agent的请求
void KnowledgeBase::handleAgentRequest(const std::string& request, const std::vector<KnowledgeRecord>& records) {
    std::string response = this->searchKnowledgeBase(records, request);
    std::cout << "Response to Agent: " << response << std::endl;
}

// int main() {
//     KnowledgeBase KnowledgeBase;
//     std::string filePath = "/home/ixu/phoenix_cuhksz_knowledge-main";
//     KnowledgeBase.loadAllFiles(filePath);
//     std::string question;
//     std::cin >> question;
//     while (question != "stop") {
//         std::cin >> question;
//         std::string response = KnowledgeBase.searchKnowledgeBase(KnowledgeBase.getRecords(), question);
//         std::cout << response << std::endl;
//     }
// }
