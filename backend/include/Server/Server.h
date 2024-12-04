//
// Created by lwt on 24-12-1.
//
#ifndef SERVER_H
#define SERVER_H

#include "../../include/Agent/Agent.h"
#include "third_party/httplib.h"
#include <unordered_map>
#include <memory>  // For std::shared_ptr
#include <string>  // For std::string
#include <nlohmann/json.hpp>  // For json parsing, ensure you have this header available

// 服务器类声明
class Server {
public:

     // 构造函数和析构函数
     Server();
     ~Server();

    // 启动服务器的函数
    void start();
    void handle_post_request(const httplib::Request& req, httplib::Response& res);
    std::string handle_message(const std::string& session_id, const std::string& message);
private:
    // 用于存储每个会话的 Agent 实例
    std::unordered_map<std::string, Agent > sessions;
};

#endif // SERVER_H
