#include "../../include/Model/Model.h"

Model::Model() {}
Model::~Model() {}

json Model::build_message(
    std::string model,
    std::string system_prompt,
    const std::vector<json>& past_messages,
    std::string new_user_content,
    const std::vector<Function>& tools
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
        {"messages", message}
    };
    if (!tools_array.empty()) {
        data["tools"] = tools_array;
    }
    return data;
}

json Model::send_message(const json message) {
    // std::cout << "body: " << message.dump() << std::endl;
    cpr::Response response = Post(
        cpr::Url{"https://api.nextapi.fun/v1/chat/completions"},
        cpr::Header{{"Authorization", "Bearer " + API_KEY}},
        cpr::Body{message.dump()}
    );
    // std::cout << "response: " << response.text << std::endl;
    if (response.status_code == 200) {
        if (json::parse(response.text)["choices"][0]["message"].contains("tool_calls")) {
            return json::parse(response.text)["choices"][0]["message"];
        }
        return json::parse(response.text)["choices"][0]["message"];
    }
    std::cerr << "请求失败: " << response.status_code << std::endl;
    return "";
}
