// Created by Gary on 24-10-23.
#include "../include/Knowledge/KnowledgeBase.h"
#include <iostream>
#include <fstream>
#include <algorithm>

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
std::string KnowledgeBase::searchKnowledgeBase(const std::vector<KnowledgeRecord>& records,
                                               const std::string& keyword1, int weight1,
                                               const std::string& keyword2, int weight2,
                                               const std::string& keyword3, int weight3) {
    std::vector<std::pair<int, KnowledgeRecord>> fileImportance;

    // 遍历所有记录
    for (const auto& record : records) {
        int importance = 0;

        // 计算第一个关键词的重要度
        int count1 = 0;
        size_t pos = record.content.find(keyword1); // 使用 find 查找子字符串
        while (pos != std::string::npos) {
            ++count1; // 找到一次关键词，计数加一
            pos = record.content.find(keyword1, pos + 1); // 查找下一个匹配项
        }
        importance += count1 * weight1;

        // 计算第二个关键词的重要度
        int count2 = 0;
        pos = record.content.find(keyword2); // 使用 find 查找第二个关键词
        while (pos != std::string::npos) {
            ++count2;
            pos = record.content.find(keyword2, pos + 1);
        }
        importance += count2 * weight2;

        // 计算第三个关键词的重要度
        int count3 = 0;
        pos = record.content.find(keyword3); // 使用 find 查找第三个关键词
        while (pos != std::string::npos) {
            ++count3;
            pos = record.content.find(keyword3, pos + 1);
        }
        importance += count3 * weight3;

        // 存储计算出来的重要度和相应的记录
        if (importance > 0) {
            fileImportance.push_back({importance, record});
        }
    }

    // 按照重要度排序，降序排列
    std::sort(fileImportance.begin(), fileImportance.end(),
              [](const std::pair<int, KnowledgeRecord>& a, const std::pair<int, KnowledgeRecord>& b) {
                  return a.first > b.first;  // 降序排列
              });

    // 返回前三个文件的内容
    std::string result;
    for (int i = 0; i < 3 && i < fileImportance.size(); ++i) {
        result += "Title: " + fileImportance[i].second.title + "\n";
        result += "Content: " + fileImportance[i].second.content + "\n\n";
    }

    if (result.empty()) {
        return "No matching record found.";
    }

    return result;
}



//处理agent的请求
// void KnowledgeBase::handleAgentRequest(const std::string& request, const std::vector<KnowledgeRecord>& records) {
//     std::string response = this->searchKnowledgeBase(records, request);
//     std::cout << "Response to Agent: " << response << std::endl;
// }

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
