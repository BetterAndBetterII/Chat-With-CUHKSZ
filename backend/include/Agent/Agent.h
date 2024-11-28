#ifndef AGENT_H
#define AGENT_H
#include <string>
#include<iostream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
using json = nlohmann::json;
class Agent {
private:
    std::vector<json> conversation_history;
    std::string system_agent(const std::string& user_input);
    std::string system_conversation_agent(const std::string& user_input, const std::string& system_input);
    std::string directly_conversation_agent(const std::string &user_input);
    const std::string API_KEY = "ak-GZTdsRjD60WUwxUrtf07b76t8K1YSbsOPiu7q01Vj0DPB9Hy";
public:
    Agent();
    ~Agent();

    std::string build_system_agent_prompt(const std::string &user_input);

    std::string build_system_conversation_agent_prompt(const std::string &user_input, const std::string &system_input);

    std::string build_directly_conversation_agent_prompt(const std::string &user_input);

    std::string handler(const std::string &user_input, const std::string &history_input);
    std::string send_message(const std::string &message);
};
#endif