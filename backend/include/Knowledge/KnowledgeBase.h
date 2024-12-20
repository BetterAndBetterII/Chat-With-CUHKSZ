#ifndef CHAT_WITH_CUHKSZ_KNOWLEDGEBASE_H
#define CHAT_WITH_CUHKSZ_KNOWLEDGEBASE_H

#include <vector>
#include <string>
#include <filesystem>

//以struct的形式储存知识
struct KnowledgeRecord {
    std::string title;
    std::string content;
};

class KnowledgeBase {
private:
    //把所有的struct放到一个vector里
    std::vector<KnowledgeRecord> records;

public:
    //获取该vector
    std::vector<KnowledgeRecord> getRecords() const;

    //读入指定路径下所有的文件
    void loadAllFiles(const std::string& basePath);

    //将文件内容写到struct里
    std::vector<KnowledgeRecord> parseFiles(const std::filesystem::path& directoryPath, const std::string& filename);

    //检索文件内容
    std::string searchKnowledgeBase(const std::vector<KnowledgeRecord>& records,
                                    const std::string& keyword1, int weight1,
                                    const std::string& keyword2, int weight2,
                                    const std::string& keyword3, int weight3);


    //处理agent请求
    std::string getKnowledge(const std::string& keyword1,
                             const std::string& keyword2,
                             const std::string& keyword3);
};

#endif // CHAT_WITH_CUHKSZ_KNOWLEDGEBASE_H
