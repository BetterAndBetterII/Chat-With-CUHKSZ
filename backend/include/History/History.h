//
// Created by lwt on 11/24/24.
//
#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <filesystem>

class History {
private:
    std::string filename;
    std::ifstream history_file;
    std::ofstream log_file;
    nlohmann::json conversation_history;

public:
    explicit History(int number = 0);
    ~History();

    std::string get_history_string() const;
    void update_history(const std::string& user_input, const std::string& response);
    void save_log_file();
};

#endif // HISTORY_H
