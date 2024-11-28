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
        std::vector<Function> tools
    );
    json send_message(json message);
private:
    const std::string API_KEY = "ak-GZTdsRjD60WUwxUrtf07b76t8K1YSbsOPiu7q01Vj0DPB9Hy";
};
#endif