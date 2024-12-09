#include "../../include/Server/Server.h"
#include "../../include/Agent/Agent.h"
#include "../../include/third_party/httplib.h"
#include "../../include/History/History.h"
#include <iostream>
#include <unordered_map>

Server::Server() = default;
Server::~Server() = default;

bool Server::login(const std::string& username, const std::string& password) {
    Agent agent(username, password);
    return agent.is_valid_login();
}

void Server::handle_post_request(const httplib::Request& req, httplib::Response& res) {
    const std::string& req_body = req.body;
    json req_json;
    try {
        req_json = json::parse(req_body.data(), req_body.data() + req_body.size());
     //   std::cout<<"test1"<<std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        res.status = 400;  // Bad Request
        res.set_content("{\"error\":\"Invalid JSON\"}", "application/json");
        return;
    }

    std::string session_id = req_json["session_id"];
    std::string message = req_json["message"];
   // std::cout<<session_id<<" "<<message<<std::endl;
    // 处理消息并获取响应
    auto result = handle_message(session_id, message);

    // 返回处理结果
    res.set_content(result, "application/json");

}

void Server::handle_get_request(const httplib::Request& req, httplib::Response& res) {
    if (req.has_param("session_id")) {
        std::string session_id = req.get_param_value("session_id");
        auto history = get_chat_history(session_id);
        res.set_content(history, "application/json");
    } else {
        auto all_first_messages = get_all_first_messages();
        res.set_content(all_first_messages, "application/json");
    }
}

std::string Server::get_chat_history(const std::string& session_id) {
    auto it = histories.find(session_id);
    if (it != histories.end()) {
        json res_json;
        res_json["session_id"] = session_id;
        res_json["history"] = it->second.get_history_string();
        return res_json.dump();
    }
    return "{\"error\": \"No history available for session ID: " + session_id + "\"}";
}

std::string Server::get_all_first_messages() {
    json res_json;
    for (const auto& [session_id, history] : histories) {
        std::string history_str = history.get_history_string();
        if (!history_str.empty()) {
            // 提取历史记录的第一条消息
            size_t pos = history_str.find('\n'); // 第一条消息到换行符的位置
            if (pos != std::string::npos) {
                res_json[session_id] = history_str.substr(0, pos);
            } else {
                res_json[session_id] = history_str;
            }
        }
    }
    return res_json.dump();
}

std::string Server::handle_message(const std::string& session_id, const std::string& message) {
    if (sessions.find(session_id) == sessions.end()) {
        sessions.emplace(session_id, Agent("default_username", "default_password"));
        histories.emplace(session_id, History(std::stoi(session_id)));
    }

    Agent& agent = sessions[session_id];
    History& history = histories[session_id];
  //  std::cout<<message<<std::endl;
    std::string response = agent.run_until_done(message);
 //   std::cout<<response<<std::endl;
    history.update_history(message, response);

    json res_json;
    res_json["session_id"] = session_id;
    res_json["response"] = response;
    return res_json.dump();
}

void Server::start() {
    httplib::Server svr;

    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        const std::string& req_body = req.body;
        try {
            json req_json = json::parse(req_body.data(), req_body.data() + req_body.size());
            std::string username = req_json["username"];
            std::string password = req_json["password"];

            bool success = login(username, password);
            if (success) {
                res.status = 200;
                std::cout<<"[POST] /login 200"<<std::endl;
                res.set_content("Login successful", "text/plain");
            } else {
                res.status = 401;
                std::cout<<"[POST] /login 401"<<std::endl;
                res.set_content("Invalid username or password", "text/plain");
            }
        } catch (const json::exception& e) {
            res.status = 400;
            std::cout<<"[POST] /login 400"<<std::endl;
            res.set_content("Invalid JSON format: " + std::string(e.what()), "text/plain");
        }
    });

    svr.Post("/chat", [&](const httplib::Request& req, httplib::Response& res) {
        handle_post_request(req, res);
        std::cout<<"[POST] /chat 200"<<std::endl;
    });

    svr.Get("/chat", [&](const httplib::Request& req, httplib::Response& res) {
        handle_get_request(req, res);
        std::cout<<"[GET] /chat 200"<<std::endl;
    });

    if (!svr.listen("localhost", 8080)) {
        std::cerr << "Error: Unable to start the server. Port may be in use." << std::endl;
    } else {
        std::cout << "Server started successfully on port 8080!" << std::endl;
    }
}
