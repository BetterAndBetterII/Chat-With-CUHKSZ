//
// Created by 34011 on 24-12-4.
//
#include "../../include/Client/Client.h"
#include "../../include/third_party/httplib.h"
#include <iostream>

Client::Client(const std::string& host, int port)
    : http_client_(host, port) {
        this->host = host;
        this->port = port;
        http_client_.set_connection_timeout(200); // 设置连接超时为200秒
        http_client_.set_read_timeout(200);       // 设置读取超时为200秒
    }

bool Client::test_connection() const
{
    try {
        httplib::Client cli(host + ":" + std::to_string(port));
        cli.set_connection_timeout(2);  // 设置2秒超时
        auto res = cli.Get("/");        // 或使用任何已知的有效端点
        return res && res->status != 0; // 只要能收到响应就认为连接正常
    } catch (...) {
        return false;
    }
}

bool Client::login(const std::string& username, const std::string& password) {

    //存储username用于session的用户分类
    this->username = username;

    json req_json;
    req_json["username"] = username;
    req_json["password"] = password;
    http_client_.set_connection_timeout(2); // 设置连接超时为2秒
    http_client_.set_read_timeout(2);       // 设置读取超时为2秒
    try
    {
        auto res = http_client_.Post("/login", req_json.dump(), "application/json");
        if (res && res->status == 200) {
            std::cout<<"Login"<<res->body<<std::endl;
            http_client_.set_connection_timeout(200); // 设置连接超时为200秒
            http_client_.set_read_timeout(200);       // 设置读取超时为200秒
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        http_client_.set_connection_timeout(200); // 设置连接超时为200秒
        http_client_.set_read_timeout(200);       // 设置读取超时为200秒
        return false;
    }
    http_client_.set_connection_timeout(200); // 设置连接超时为200秒
    http_client_.set_read_timeout(200);       // 设置读取超时为200秒
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
        std::cout<<"Client: Get Chat History: "<<session_id<<" \nResponse: "<<res->body<<std::endl;
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
        std::cout<<"Client: Get First Messages: \nResponse: "<<res->body<<std::endl;
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