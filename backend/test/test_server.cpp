#include <iostream>
#include "../include/third_party/httplib.h" // 需要链接 httplib 库
#include "nlohmann/json.hpp"  // 用于解析 JSON 请求和响应
#include "../include/Server/Server.h"
using json = nlohmann::json;
#include <iostream>
#include <thread>
#include "../../backend/include/Server/Server.h"

#include "../../frontend/include/Client/Client.h"

Client::Client(const std::string& host, int port)
    : http_client_(host, port) {}

bool Client::login(const std::string& username, const std::string& password) {

    //存储username用于session的用户分类
    this->username = username;

    json req_json;
    req_json["username"] = username;
    req_json["password"] = password;
    //std::cout<<"test"<<std::endl;
    auto res = http_client_.Post("/login", req_json.dump(), "application/json");
    if (res && res->status == 200) {
        std::cout<<"Login"<<res->body<<std::endl;
        return true;
    }
    return false;
}

std::string Client::send_message(const std::string& session_id, const std::string& message) {
    json req_json;
    req_json["session_id"] = bind_to_username(session_id);
    req_json["message"] = message;

    auto res = http_client_.Post("/chat", req_json.dump(), "application/json");
    if (res && res->status == 200) {
        std::cout<<"Send Message: "<<session_id<<" "<<message<<" \nResponse: "<<res->body<<std::endl;
        return res->body;
    }

    return "Error:1 " + (res ? std::to_string(res->status) : "No response");
}
std::string Client::get_chat_history(const std::string& session_id) {
    auto res = http_client_.Get(("/chat?session_id=" + bind_to_username(session_id)).c_str());
    if (res && res->status == 200) {
        std::cout<<"Server: Get Chat History: "<<session_id<<" \nResponse: "<<res->body<<std::endl;
        return res->body;
    }
    return "Error: " + (res ? std::to_string(res->status) : "No response");
}

std::string Client::get_first_messages() {
    auto res = http_client_.Get(("/chat?username=" + username).c_str());
    if (res && res->status == 200) {
        // {
        //     "123090848/767": "(user: HI!)"
        // }
        std::cout<<"Get First Messages: \nResponse: "<<res->body<<std::endl;
        return res->body;
    }
    return "Error: " + (res ? std::to_string(res->status) : "No response");
}

std::string Client::bind_to_username(const std::string& session_id){
    if (session_id.find("/") == -1) {
        std::cout << "Client: bind " << session_id << " to username-> " << username+"/"+session_id <<std::endl;
        return username + "/" + session_id;
    }
    return session_id;
}


int main() {
    Server server;
    std::thread server_thread([&]() {
      server.start();
    });

    // 等待服务器启动
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Client client("localhost", 8080);

    //basic info input
    std::string username = "";
    std::string password = "";
    const char* env_username = std::getenv("STUDENT_USERNAME");
    const char* env_password = std::getenv("STUDENT_PASSWORD");
    if (env_username) {
        username = env_username;
    } else {
        std::cout << "Username:" << std::endl;
        std::getline(std::cin, username);
    }
    if (env_password) {
        password = env_password;
    } else {
        std::cout << "Password:" << std::endl;
        std::getline(std::cin, password);
    }

    std::string session_id;
    std::string message;

    if (client.login(username, password)) {
        std::cout << "Login success!" << std::endl;
        std::cout << "----Test send message----" << std::endl;
        int i = 2;
        while (i--) {
            std::cout << "Session_id?" << std::endl;
            std::getline(std::cin, session_id);
            std::cout << "message?" << std::endl;
            std::getline(std::cin, message);
            std::string response = client.send_message(session_id, message);
            std::cout << "Server response: \n" << response << std::endl;

        }

        // 获取所有会话的第一条消息
        std::cout << "----Test get first message----" << std::endl;
        std::string first_messages = client.get_first_messages();
        std::cout << "Client: All first messages: \n" << first_messages << std::endl;

        // 获取当前会话的历史记录
        std::cout << "----Test get history----" << std::endl;
        std::string chat_history = client.get_chat_history(session_id);
        std::cout << "Client: Chat history: \n" << chat_history << std::endl;

    } else {
        std::cout << "Login failed!" << std::endl;
    }

    return 0;
}
