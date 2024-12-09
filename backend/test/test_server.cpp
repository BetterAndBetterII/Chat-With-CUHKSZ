#include <iostream>
#include "../include/third_party/httplib.h" // 需要链接 httplib 库
#include "nlohmann/json.hpp"  // 用于解析 JSON 请求和响应
#include "../include/Server/Server.h"
using json = nlohmann::json;
#include <iostream>
#include <thread>
#include "../../backend/include/Server/Server.h"

class Client {
public:
    Client(const std::string& host, int port);

    bool login(const std::string& username, const std::string& password);
    std::string send_message(const std::string& session_id, const std::string& message);
    std::string get_chat_history(const std::string& session_id);
    std::string get_first_messages();

private:
    httplib::Client http_client_;
};

Client::Client(const std::string& host, int port)
    : http_client_(host, port) {}

bool Client::login(const std::string& username, const std::string& password) {
    json req_json;
    req_json["username"] = username;
    req_json["password"] = password;
  //  std::cout<<"test"<<std::endl;
    auto res = http_client_.Post("/login", req_json.dump(), "application/json");
    if (res && res->status == 200) {
        return true; // 根据服务器的实现，状态码200即登录成功
    }
    return false;
}

std::string Client::send_message(const std::string& session_id, const std::string& message) {
    json req_json;
    req_json["session_id"] = session_id;
    req_json["message"] = message;
//    std::cout<<session_id<<" "<<message<<std::endl;

    auto res = http_client_.Post("/chat", req_json.dump(), "application/json");
    if (res && res->status == 200) {
        return res->body;
    }

    return "Error:1 " + (res ? std::to_string(res->status) : "No response");
}

std::string Client::get_chat_history(const std::string&session_id) {
    auto res = http_client_.Get(("/chat?session_id=" + session_id).c_str());
    if (res && res->status == 200) {
        return res->body;
    }
    return "Error: " + (res ? std::to_string(res->status) : "No response");
}

std::string Client::get_first_messages() {
    auto res = http_client_.Get("/chat");
    if (res && res->status == 200) {
        return res->body;
    }
    return "Error: " + (res ? std::to_string(res->status) : "No response");
}

int main() {
    Server server;
    std::thread server_thread([&]() {
        server.start();
    });

    // 等待服务器启动
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Client client("localhost", 8081);

    std::string username = "123090337";
    std::string password = "05211224Lu!!";

    if (client.login(username, password)) {
  //      std::cout << "Login Successfully" << std::endl;
        const std::string& session_id = "1";
        std::string response = client.send_message(session_id, "Hello, Server!");
        std::cout << "Server response: " << response << std::endl;

        // 获取当前会话的历史记录
        std::string chat_history = client.get_chat_history(session_id);
        std::cout << "Chat history: " << chat_history << std::endl;

        // 获取所有会话的第一条消息
        std::string first_messages = client.get_first_messages();
        std::cout << "All first messages: " << first_messages << std::endl;
    } else {
        std::cout << "Login failed!" << std::endl;
    }

    server_thread.join();
    while (1) {
        const std::string& session_id = "1";
        std::string mes;
        std::cin>>mes;
        std::string response = client.send_message(session_id, mes);
        std::cout << "Server response: " << response << std::endl;

    }
    return 0;
}
