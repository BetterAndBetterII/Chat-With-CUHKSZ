//
// Created by 34011 on 24-12-4.
//
#include "client.h"
#include "../include/Server/Server.h"
#include <iostream>

Client::Client(const std::string& host, int port)
    : http_client_(host, port) {}

bool Client::login(const std::string& username, const std::string& password) {
    json req_json;
    req_json["username"] = username;
    req_json["password"] = password;

    auto res = http_client_.Post("/login", req_json.dump(), "application/json");
    if (res && res->status == 200) {
        auto res_json = json::parse(res->body);
        return res_json.value("success", false);
    }

    return false;
}

std::string Client::send_message(const std::string& session_id, const std::string& message) {
    json req_json;
    req_json["session_id"] = session_id;
    req_json["message"] = message;

    auto res = http_client_.Post("/chat", req_json.dump(), "application/json");
    if (res && res->status == 200) {
        return res->body;
    }

    return "Error: " + (res ? std::to_string(res->status) : "No response");
}

int main() {
    Client client("localhost", 8081);

    std::string username = "user123";
    std::string password = "pass123";

    if (client.login(username, password)) {
      	std::cout << "Login Successfully" << std::endl;
        std::string session_id = "session_user123";
        std::string response = client.send_message(session_id, "Hello, Server!");
        std::cout << "Server response: " << response << std::endl;

        // 获取当前会话的历史记录
        httplib::Client http_client("localhost", 8081);
        auto res = http_client.Get(("/history?session_id=" + session_id).c_str());
        if (res && res->status == 200) {
            std::cout << "Chat history: " << res->body << std::endl;
        }

        // 获取所有会话的第一条消息
        res = http_client.Get("/first_messages");
        if (res && res->status == 200) {
            std::cout << "All first messages: " << res->body << std::endl;
        }
    } else {
        std::cout << "Login failed!" << std::endl;
    }

    return 0;
}