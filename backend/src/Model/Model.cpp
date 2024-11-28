#include "../../include/Model/Model.h"
Model::Model() {}
Model::~Model() {}

json Model::build_message(
    std::string model,
    std::string system_prompt,
    const std::vector<std::string>& past_messages,
    std::string new_user_content,
    json tools
) {
    json message = json::parse(past_messages);
    message.push_back({{"role", "system"}, {"content", system_prompt}});
    message.push_back({{"role", "user"}, {"content", new_user_content}});
    json data = {
        {"model", model},
        {"messages", message},
        {"tools", tools}
    };
    return data;
}

std::string Model::send_message(const json message) {
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