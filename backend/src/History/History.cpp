#include "../../include/History/History.h"
#include <iostream>
#include <filesystem>

History::History(int number) {
    // ȷ�� history �ļ��д���
    std::string history_folder = "frontend/resources/History";
    if (!std::filesystem::exists(history_folder)) {
        std::filesystem::create_directory(history_folder);
    }

    if (number != 0) {
        // ͨ��ָ����ż�����ʷ��¼
        filename = history_folder + "/conversation_log" + std::to_string(number) + ".json";
        history_file.open(filename);
        if (history_file.is_open()) {
            try {
                history_file >> conversation_history;
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "������ʷ��¼�ļ�ʧ�ܣ���ʼ��Ϊ����ʷ��¼��" << e.what() << std::endl;
                conversation_history = nlohmann::json::object();
            }
            history_file.close();
        } else {
            std::cerr << "�޷���ȡ��ʷ��¼�ļ����ļ�����Ϊ�ջ򲻴��ڣ���ʼ��Ϊ����ʷ��¼��" << std::endl;
            conversation_history = nlohmann::json::object();
        }
    } else {
        // �Զ�Ѱ����һ��δʹ�õ���־�ļ����
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

History::~History() {
    save_log_file();
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
        std::cerr << "δ�ҵ��Ի���¼��" << std::endl;
    }
    return history_str;
}

void History::update_history(const std::string& user_input, const std::string& response) {
    nlohmann::json user_message = {{"role", "user"}, {"content", user_input}};
    nlohmann::json assistant_message = {{"role", "assistant"}, {"content", response}};
    conversation_history["messages"].push_back(user_message);
    conversation_history["messages"].push_back(assistant_message);
}

void History::save_log_file() {
    log_file.open(filename);
    if (!log_file.is_open()) {
        std::cerr << "�޷�����־�ļ���" << filename << std::endl;
        return;
    }
    log_file << conversation_history.dump(4);  // ���� 4 �񱣴�
    log_file.close();
}
