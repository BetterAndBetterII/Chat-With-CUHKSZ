//
// Created by yf on 11/13/24.
//
#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <format>  // For C++20 and above

using json = nlohmann::json;

class Model {
    std::string model_choice = "gpt-4o-mini";
public:
    // 返回响应内容的函数
    [[nodiscard]] std::string get_response(const std::string &user_input) const;
};

#endif // MODEL_H
