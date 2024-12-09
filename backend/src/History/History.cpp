#include "../../include/History/History.h"
#include <iostream>
#include <fstream>
#include <filesystem>

History::History(int number) {
   // std::string history_path = get_absolute_path("frontend/resources/History");
    std::string history_folder = "D:\Code\c++\Visual Studio\Chat-With-CUHKSZ";
   // ../../../../frontend/resources/History

    ensure_history_folder_exists(history_folder);

    if (number != 0) {
        // Load specified log file
        filename = history_folder + "/conversation_log" + std::to_string(number) + ".json";
        std::ifstream history_file(filename);
        if (history_file.is_open()) {
            try {
                history_file >> conversation_history;
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "Failed to parse history file, initializing as empty history: " << e.what() << std::endl;
                conversation_history = nlohmann::json::object();
            }
        } else {
            std::cerr << "Cannot read history file! Initializing as empty history." << std::endl;
            conversation_history = nlohmann::json::object();
        }
    } else {
        // Find next available log file number
        number = 1;
        do {
            filename = history_folder + "/conversation_log" + std::to_string(number) + ".json";
            if (std::filesystem::exists(filename)) {
                ++number;
            } else {
                break;
            }
        } while (true);
        conversation_history = nlohmann::json::object();
    }
}

std::string History::get_history_string() const {
    std::string history_str;
    if (conversation_history.contains("messages")) {
        for (const auto& message : conversation_history["messages"]) {
            std::string role = message["role"];
            std::string content = message["content"];
            history_str.append("(").append(role).append(": ").append(content).append(")\n");
        }
    } else {
        std::cerr << "No conversation history found." << std::endl;
    }
    return history_str;
}

void History::update_history(const std::string& user_input, const std::string& response) {
    nlohmann::json user_message = {{"role", "user"}, {"content", user_input}};
    nlohmann::json assistant_message = {{"role", "assistant"}, {"content", response}};
    conversation_history["messages"].push_back(user_message);
    conversation_history["messages"].push_back(assistant_message);
}

void History::save_log_file() const {
    std::ofstream log_file(filename);
    if (!log_file.is_open()) {
        std::cerr << "Unable to open log file: " << filename << std::endl;
        return;
    }
    log_file << conversation_history.dump(4); // Save with 4-space indentation
}

void History::ensure_history_folder_exists(const std::string& folder) const {
    if (!std::filesystem::exists(folder)) {
        std::filesystem::create_directory(folder);
    }
}

std::string History::get_absolute_path(const std::string& relative_path) {
    return (std::filesystem::current_path() / relative_path).string();
}

