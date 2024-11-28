#include "../../include/Model/Model.h"
#include <iostream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
Model::Model() {}
Model::~Model() {}
std::string Model::send_message(const std::string& message) {
    json request_body = {
        {"model", "gpt-4o"},
        {"messages", message}
    };
    cpr::Response response = cpr::Post(
        cpr::Url{"https://api.nextapi.fun/v1/chat/completions"},
        cpr::Header{{"Authorization", "Bearer " + API_KEY}},
        cpr::Body{request_body.dump()}
    );
    if (response.status_code == 200) {
        auto reply = json::parse(response.text)["choices"][0]["message"]["content"];
        return reply;
    } else {
        std::cerr << "请求失败: " << response.status_code << std::endl;
        return "";
    }
}