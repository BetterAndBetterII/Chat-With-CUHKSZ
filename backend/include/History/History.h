#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>

class History {
public:
    explicit History(int number = 0);
    ~History() = default;

    std::string get_history_string() const;
    void update_history(const std::string& user_input, const std::string& response);
    void save_log_file() const;

private:
    std::string filename;
    nlohmann::json conversation_history;

    void ensure_history_folder_exists(const std::string& folder) const;
    std::string get_absolute_path(const std::string& relative_path);
};

#endif // HISTORY_H
