#include "../../include/History/History.h"

History::History(int number) {
    if (number != 0) {
        filename = "conversation_log" + std::to_string(number) + ".json";
        history_file.open(filename);
        if (history_file.is_open()) {
            try {
                history_file >> conversation_history;
            }
            catch (const nlohmann::json::parse_error& e) {
                std::cerr << "解析历史记录文件失败，初始化为空历史记录：" << e.what() << std::endl;
                conversation_history = nlohmann::json::object();
            }
            history_file.close();
        }
        else {
            std::cerr << "无法读取历史记录文件！文件可能为空或不存在，初始化为空历史记录。" << std::endl;
            conversation_history = nlohmann::json::object();
        }
    }
    else {
        number = 1;
        do {
            filename = "conversation_log" + std::to_string(number) + ".json";
            if (std::filesystem::exists(filename)) {
                ++number;
            }
            else {
                break;
            }
        } while (true);
        conversation_history = nlohmann::json::object();
    }
}

History::~History() {
    save_log_file();
}

std::string History::get_history_string() const {
    std::string history_str;
    if (conversation_history.contains("messages")) {
        for (const auto& message : conversation_history["messages"]) {
            std::string role = message["role"];
            std::string content = message["content"];
            history_str.append("(").append(role).append(": ").append(content).append(")");
        }
    }
    else {
        std::cerr << "未找到对话记录。" << std::endl;
    }
    return history_str;
}

void History::update_history(const std::string& user_input, const std::string& response) {
    nlohmann::json user_message;
    user_message["role"] = "user";
    user_message["content"] = user_input;
    conversation_history["messages"].push_back(user_message);

    nlohmann::json assistant_message;
    assistant_message["role"] = "assistant";
    assistant_message["content"] = response;
    conversation_history["messages"].push_back(assistant_message);
}

void History::save_log_file() {
    log_file.open(filename);
    if (!log_file.is_open()) {
        std::cerr << "无法打开日志文件！" << std::endl;
    }
    log_file << conversation_history.dump(4);
    log_file.close();
}
