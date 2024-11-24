//
// Created by yf on 11/13/24.
//
#ifndef AGENT_H
#define AGENT_H

#include <string>
#include "history.h"

class Agent {
private:
    History history_manager;

public:
    explicit Agent(int number);
    ~Agent();

    std::string system_agent(const std::string& user_input, const std::string& history) const;
    std::string conversation_agent(const std::string& user_input, const std::string& history) const;
    std::string handler(const std::string& user_input);
};

#endif // AGENT_H

