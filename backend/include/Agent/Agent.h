//
// Created by yf on 11/13/24.
//
#ifndef AGENT_H
#define AGENT_H

#include <string>
#include <set>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "../../include/Model/Model.h"

class Agent {
    std::ofstream log_file;  // 用于存储历史记录
    std::ifstream history_file;  // 用于读取历史记录
    Model model;
    std::string filename;
    nlohmann::json conversation_history;
    std::set<std::string> system_list = {"teaching_system", "library_system", "homework_system"};

    std::string system_agent(const std::string& user_input, const std::string& history) const;
    std::string conversation_agent(const std::string& user_input, const std::string& history) const;
    void history_file_manager(int number);
    std::string history_string_maker();
    void conversation_history_updater(const std::string& user_input, const std::string& response);
    void log_file_saver();

public:
    explicit Agent(int number = 0);
    ~Agent();
    std::string handler(const std::string& user_input);
};

#endif // AGENT_H
