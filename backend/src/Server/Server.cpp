#include "../../include/Server/Server.h"  // 引入头文件，而不是重复定义类
#include "../../include/Agent/Agent.h"
#include "../../include/third_party/httplib.h"
#include <iostream>
#include <unordered_map>

Server::Server() = default;
Server::~Server() = default;
void Server::handle_post_request(const httplib::Request& req, httplib::Response& res) {
    // 解析请求体中的 JSON 数据
    json req_json = json::parse(req.body);

    std::string session_id = req_json["session_id"];
    std::string message = req_json["message"];

    // 处理消息
    auto result = handle_message(session_id, message);

    // 返回处理结果
    res.set_content(result, "text/plain");
}
std::string Server::handle_message(const std::string& session_id, const std::string& message) {
    // 检查是否存在该会话
    auto it = sessions.find(session_id);
    if (it == sessions.end()) {
        // 如果会话不存在，创建新的会话
        sessions.emplace(session_id, Agent("default_username", "default_password"));
    }

    // 获取当前会话的 Agent 实例
    Agent& agent = sessions[session_id];

    // 调用 agent 的 run_until_done 方法处理消息
    auto result =  agent.run_until_done(message);
    std::cout<<result<<std::endl;
    return result;
}
void Server::start() {
    httplib::Server svr;

    // API: 获取历史对话并进行对话
    svr.Post("/chat", [&](const httplib::Request& req, httplib::Response& res) {
       handle_post_request(req, res);  // 调用处理函数
    });
    std::cout << "Starting server..." << std::endl;
    // 启动服务器监听 8080 端口
    if (!svr.listen("0.0.0.0", 3334)) {
        std::cerr << "Error: Unable to start the server. Port may be in use." << std::endl;
        return;
    }

    std::cout << "Server stopped." << std::endl;
}
