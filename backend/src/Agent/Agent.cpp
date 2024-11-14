#include "../../include/Agent/Agent.h"

Agent::Agent(const int number) {
    history_file_manager(number);
}

Agent::~Agent() {
    log_file_saver();
}

std::string Agent::system_agent(const std::string& user_input, const std::string& history) const {
    const std::string input = "history: [" + history + "], current_input: [" + user_input + "]";
    std::string prompt = R"(
You are one of the best helper for students, you will be given the question from the student and the conversation history; Your job is to answer the question from students. You can select a system for help;
)";
    prompt += R"(
; Here are the system you can choose: teaching_system, library_system, homework_system; teaching_system is for class schedule and class selection; homework_system is for homework; library_system is for books borrowing and the reservation for discussion room. Here is the conversation history and the current question from students:
)" + input + R"(
; You MUST reply the name of the system directly without any other words; If you can answer the question directly without the help of those system, you MUST answer no_need_help.
)";
    std::erase(prompt, '\n');
    // std::cout<<prompt<<std::endl;
    std::string response = model.get_response(prompt);
    // std::cout<<"manager: "<<response<<std::endl;
    return response;
}

std::string Agent::conversation_agent(const std::string& user_input, const std::string& history) const {
    std::string prompt = "history: [" + history + "], current_input: [" + user_input + "]";
    std::erase(prompt, '\n');
    // std::cout<<prompt<<std::endl;
    std::string response = model.get_response(prompt);
    // std::cout<<"conversation: "<<response<<std::endl;
    return response;
}

void Agent::history_file_manager(int number) {
    if (number != 0) {
        filename = "conversation_log" + std::to_string(number) + ".json";
        history_file.open(filename);
        if (history_file.is_open()) {
            try {
                history_file >> conversation_history;  // 读取现有的JSON数据
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "解析历史记录文件失败，初始化为空历史记录：" << e.what() << std::endl;
                conversation_history = nlohmann::json::object();
            }
            history_file.close();
        } else {
            std::cerr << "无法读取历史记录文件！文件可能为空或不存在，初始化为空历史记录。" << std::endl;
            conversation_history = nlohmann::json::object();  // 初始化为空数组
        }
    } else {
        number = 1;
        do {
            filename = "conversation_log" + std::to_string(number) + ".json";
            if (std::filesystem::exists(filename)) {
                ++number;  // 如果文件存在，则递增编号
            } else {
                break;  // 如果文件不存在，则退出循环
            }
        } while (true);
        conversation_history = nlohmann::json::object();
    }
}

std::string Agent::history_string_maker() {
    std::string history_str;
    if (conversation_history.contains("messages")) {
        for (const auto& message : conversation_history["messages"]) {
            std::string role = message["role"];
            std::string content = message["content"];
            history_str.append("(").append(role).append(": ").append(content).append(")");
        }
    } else {
        std::cerr << "未找到对话记录。" << std::endl;
    }
    return history_str;
}

void Agent::conversation_history_updater(const std::string& user_input, const std::string& response) {
    nlohmann::json user_message;
    user_message["role"] = "user";
    user_message["content"] = user_input;
    conversation_history["messages"].push_back(user_message);

    nlohmann::json assistant_message;
    assistant_message["role"] = "assistant";
    assistant_message["content"] = response;
    conversation_history["messages"].push_back(assistant_message);
}

void Agent::log_file_saver() {
    log_file.open(filename);
    if (!log_file.is_open()) {
        std::cerr << "无法打开日志文件！" << std::endl;
    }
    log_file << conversation_history.dump(4);  // 格式化输出
    log_file.close();  // 关闭文件
}

std::string Agent::handler(const std::string& user_input) {
    const std::string history_str = history_string_maker();
    std::string response = system_agent(user_input, history_str);
    if (system_list.contains(response)) {
        if (response == "teaching_system") {
            std::cout << "call teaching_system" << std::endl;
        }
        if (response == "library_system") {
            std::cout << "call library system" << std::endl;
        }
        if (response == "homework_system") {
            std::cout << "call homework_system" << std::endl;
        }
    } else {
        response = conversation_agent(user_input, history_str);
    }
    conversation_history_updater(user_input, response);
    return response;
}
