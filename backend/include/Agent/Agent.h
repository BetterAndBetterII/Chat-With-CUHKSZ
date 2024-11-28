#ifndef AGENT_H
#define AGENT_H
#include <string>
#include<iostream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include "../System/Tools.h"
#include "../Model/Model.h"

using json = nlohmann::json;
class Agent {
private:
    int MAX_LOOP_COUNT = 3;
    std::string EXIT_SIGNAL = "<exit>";
    std::vector<json> conversation_history;
    std::string system_prompt = "You are a chatbot that can call tools to help the user with tasks. If you or the toolcall result have answered user's question, you must summary what you have done with tools results and add " + EXIT_SIGNAL + " at end of the conversation.";
    Model model;
    Tools tools;
public:
    Agent();
    ~Agent();

    std::vector<Function> get_tools();

    void insert_memory(const std::string& message, const std::string& role);

    std::string run(const std::string &message, bool enable_tools);

    std::string run_until_done(const std::string &message);
};
#endif