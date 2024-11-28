#ifndef MODEL_H
#define MODEL_H
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <cpr/cpr.h>
using json = nlohmann::json;
class Model {
public:
    Model();
    ~Model();
    std::string send_message(json message);
private:
    const std::string API_KEY = "ak-GZTdsRjD60WUwxUrtf07b76t8K1YSbsOPiu7q01Vj0DPB9Hy";
};
#endif