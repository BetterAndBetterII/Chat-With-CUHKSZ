#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

class History {
public:
    explicit History(int number = 0);  // 如果未指定编号，自动生成文件名
    ~History();

    std::string get_history_string() const;  // 获取历史记录的字符串表示
    void update_history(const std::string& user_input, const std::string& response);  // 更新对话记录
    void save_log_file();  // 保存日志文件

private:
    std::string filename;                   // 当前对话日志文件路径
    nlohmann::json conversation_history;    // 存储历史记录的 JSON 对象
    std::ifstream history_file;             // 用于读取历史记录
    std::ofstream log_file;                 // 用于写入日志文件
};

#endif // HISTORY_H
