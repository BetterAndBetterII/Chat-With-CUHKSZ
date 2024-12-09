#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <nlohmann/json.hpp> // 使用 json 库（如 nlohmann/json）
#include "../third_party/httplib.h" // 使用 httplib 库

using json = nlohmann::json;

class Client {
public:
    // 构造函数，初始化客户端
    Client(const std::string& host, int port);

    // 登录接口
    bool login(const std::string& username, const std::string& password);

    // 发送消息接口
    std::string send_message(const std::string& session_id, const std::string& message);

    // 获取当前会话的聊天记录接口
    std::string get_chat_history(const std::string& session_id);

    // 获取所有会话的第一条消息
    std::string get_first_messages();

private:
    httplib::Client http_client_; // HTTP 客户端，用于与服务器交互
};

#endif // CLIENT_H
