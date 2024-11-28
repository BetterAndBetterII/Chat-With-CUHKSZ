#include "../../include/Model/Model.h"

#include "../System/Tools.h"
Model::Model() {}
Model::~Model() {}

json Model::build_message(
    std::string model,
    std::string system_prompt,
    const std::vector<json>& past_messages,
    std::string new_user_content,
    std::vector<Function> tools
) {
    std::vector<json> message = past_messages;
    message.push_back({{"role", "system"}, {"content", system_prompt}});
    message.push_back({{"role", "user"}, {"content", new_user_content}});
    
    // 将 tools 转换为 json 数组
    json tools_array = json::array();
    for (const auto& tool : tools) {
        tools_array.push_back(tool.to_json());
    }
    
    json data = {
        {"model", model},
        {"messages", message},
        {"tools", tools_array}
    };
    return data;
}

std::string Model::send_message(const json message) {
    cpr::Response response = Post(
        cpr::Url{"https://api.nextapi.fun/v1/chat/completions"},
        cpr::Header{{"Authorization", "Bearer " + API_KEY}},
        cpr::Body{message.dump()}
    );
    std::cout << "body: " << message.dump() << std::endl;
    std::cout << "response: " << response.text << std::endl;
    if (response.status_code == 200) {
        auto reply = json::parse(response.text)["choices"][0]["message"]["content"];
        return reply;
    }
    std::cerr << "请求失败: " << response.status_code << std::endl;
    return "";
}
