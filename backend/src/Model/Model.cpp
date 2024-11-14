#include "../../include/Model/Model.h"

// 函数：构建请求体
std::string create_request_body(const std::string& model_choice, const std::string& user_input) {
    return std::format(R"({{"model": "{}", "messages": [{{"role": "user", "content": "{}"}}]}})",
                       model_choice, user_input);
}

std::string Model::get_response(const std::string &user_input) const {
    // 使用create_request_body函数构造请求体
    const std::string body = create_request_body(model_choice, user_input);

    // API调用
    cpr::Response r = Post(
        cpr::Url{"https://api.nextapi.fun/v1/chat/completions"},
        cpr::Header{{"Authorization", "Bearer ak-GZTdsRjD60WUwxUrtf07b76t8K1YSbsOPiu7q01Vj0DPB9Hy"}},
        cpr::Body{body},
        cpr::Header{{"Content-Type", "application/json"}}
    );

    // 检查响应状态
    if (r.status_code == 200) {
        // 解析JSON响应
        try {
            json response_json = json::parse(r.text);
            // 提取内容字段
            std::string content = response_json["choices"][0]["message"]["content"];
            return content;
        } catch (const json::exception &e) {
            return "Error parsing JSON response: " + std::string(e.what());
        }
    }
    return "Error: " + std::to_string(r.status_code) + " - " + r.text;
}
