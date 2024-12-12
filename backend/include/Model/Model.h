#ifndef MODEL_H
#define MODEL_H
#include <string>
#include <nlohmann/json.hpp>
#include "../System/Tools.h"
#include <iostream>
#include <cpr/cpr.h>
class Function;
using json = nlohmann::json;
class Model {
public:
    Model();
    ~Model();
    json build_message(
        std::string model,
        std::string system_prompt,
        const std::vector<json>& past_messages,
        std::string new_user_content,
        const std::vector<Function>& tools
    );
    json send_message(json message);
private:
    const std::string API_BASE = std::getenv("APIBASE") ? std::getenv("APIBASE") : "https://api.nextapi.fun/v1/chat/completions";
    const std::string API_KEY = std::getenv("APIKEY");
};
#endif