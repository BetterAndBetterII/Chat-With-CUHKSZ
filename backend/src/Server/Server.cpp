#include "../../include/Server/Server.h"
#include "../../include/Agent/Agent.h"
#include "../../include/third_party/httplib.h"
#include "../../include/History/History.h"
#include <iostream>
#include <unordered_map>

Server::Server() = default;
Server::~Server() = default;

bool Server::login(const std::string& username, const std::string& password) {
    std::cout<<"username"<<username<< "password"<<password<<std::endl;
    Agent agent(username, password);
    return agent.is_valid_login();
}

void Server::handle_post_request(const httplib::Request& req, httplib::Response& res) {
    const std::string& req_body = req.body;
    try {
        json req_json = json::parse(req_body.data(), req_body.data() + req_body.size());
        std::string session_id = req_json["session_id"];
        std::string message = req_json["message"];

        // 处理消息并获取响应
        auto result = handle_message(session_id, message);

        // 返回处理结果
        res.set_content(result, "application/json");
    } catch (const json::exception& e) {
        res.status = 400;
        res.set_content("Invalid JSON format: " + std::string(e.what()), "text/plain");
    }
}

void Server::handle_get_request(const httplib::Request& req, httplib::Response& res) {
    if (req.has_param("session_id")) {
        // 获取指定会话的历史记录
        std::string session_id = req.get_param_value("session_id");
        auto history = get_chat_history(session_id);
        res.set_content(history, "application/json");
    } else {
        // 获取所有会话的第一条消息
        auto all_first_messages = get_all_first_messages();
        res.set_content(all_first_messages, "application/json");
    }
}

std::string Server::get_chat_history(const std::string& session_id) {
    auto it = histories.find(session_id);
    if (it != histories.end()) {
        return it->second.get_history_string();
    }
    return "No history available for session ID: " + session_id;
}

std::string Server::get_all_first_messages() {
    json res_json;
    for (const auto& [session_id, agent] : sessions) {
        auto history = get_chat_history(session_id);
        if (!history.empty()) {
            res_json[session_id] = history.front(); // 获取每个会话的第一条消息
        }
    }
    return res_json.dump();
}

std::string Server::handle_message(const std::string& session_id, const std::string& message) {
    // 如果会话不存在，则创建新会话并初始化 History
    if (sessions.find(session_id) == sessions.end()) {
        sessions.emplace(session_id, Agent("default_username", "default_password"));
        histories.emplace(session_id, History());  // 为每个会话创建独立的历史记录
    }

    // 获取会话对应的 Agent 和 History
    Agent& agent = sessions[session_id];
    History& history = histories[session_id];

    // 处理消息并记录历史
    std::string response = agent.run_until_done(message);
    history.update_history(message, response);

    return response;
}
void Server::start() {
    httplib::Server svr;

    // POST 请求: 登录
    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        const std::string& req_body = req.body;
        try {
            // 解析请求体
            json req_json = json::parse(req_body.data(), req_body.data() + req_body.size());
            std::string username = req_json["username"];
            std::string password = req_json["password"];
            std::cout<<"username"<<username<< "password"<<password<<std::endl;
            // 处理登录
            bool success = login(username, password);
            if (success) {
                res.status = 200;
                res.set_content("Login successful", "text/plain");
            } else {
                res.status = 401;  // 未授权
                res.set_content("Invalid username or password", "text/plain");
            }
        } catch (const json::exception& e) {
            res.status = 400;
            res.set_content("Invalid JSON format: " + std::string(e.what()), "text/plain");
        }
    });

    // POST 请求: 处理聊天消息
    svr.Post("/chat", [&](const httplib::Request& req, httplib::Response& res) {
        handle_post_request(req, res);  // 调用已定义的函数
    });

    // GET 请求: 获取历史记录或所有会话的第一条消息
    svr.Get("/chat", [&](const httplib::Request& req, httplib::Response& res) {
        handle_get_request(req, res);  // 调用已定义的函数
    });

    // 启动服务器
    if (!svr.listen("0.0.0.0", 8081)) {
        std::cerr << "Error: Unable to start the server. Port may be in use." << std::endl;
        return;
    }
    std::cout << "Server started successfully on port 8081!" << std::endl;
}
