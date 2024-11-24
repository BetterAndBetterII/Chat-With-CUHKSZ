#include "../../include/Agent/Agent.h"
#include "../../include/History/History.h"

Agent::Agent(const int number) : history_manager(number) {}

Agent::~Agent() {}

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
    std::string response = model.get_response(prompt);
    return response;
}

std::string Agent::conversation_agent(const std::string& user_input, const std::string& history) const {
    std::string prompt = "history: [" + history + "], current_input: [" + user_input + "]";
    std::erase(prompt, '\n');
    std::string response = model.get_response(prompt);
    return response;
}

std::string Agent::handler(const std::string& user_input) {
    const std::string history_str = history_manager.get_history_string();
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
    }
    else {
        response = conversation_agent(user_input, history_str);
    }
    history_manager.update_history(user_input, response);
    return response;
}
