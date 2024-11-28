#include "../../include/Agent/Agent.h"
Agent::Agent() {}
Agent::~Agent() {}
std::string Agent::build_system_agent_prompt(const std::string &user_input) {
    std::stringstream ss;
    ss  << user_input;
    return ss.str();
}
std::string Agent::build_system_conversation_agent_prompt(const std::string &user_input,const std::string &system_input) {
    std::stringstream ss;
    ss  << user_input;
    return ss.str();
}
std::string Agent::build_directly_conversation_agent_prompt(const std::string &user_input) {
    std::stringstream ss;
    ss  << user_input;
    return ss.str();
}
std::string Agent::system_agent(const std::string &user_input) {
    std::string input = build_system_agent_prompt(user_input);
    std::string response = send_message(input);
    return response;
}
std::string Agent::system_conversation_agent(const std::string& user_input, const std::string& system_input) {
    std::string input = build_system_conversation_agent_prompt(user_input,system_input);
    std::string response = send_message(input);
    return response;
}
std::string Agent::directly_conversation_agent(const std::string& user_input) {
    std::string input = build_directly_conversation_agent_prompt(user_input);
    std::string response = send_message(input);
    return response;
}
std::string Agent::handler(const std::string& user_input, const std::string& history_input) {
    std::string system_input;
    std::string output;
    std::string choice="null";
    // choice = system_agent(input);
    if (choice == "null") {
        output = directly_conversation_agent(user_input);
    } else {
        if (choice == "library") {
            system_input = " ";
        }
        output = system_conversation_agent(user_input, system_input);
    }
    return output;
}
std::string Agent::send_message(const std::string& message) {
    std::cout<<message;
    json user_message = {
        {"role", "user"},
        {"content", message}
    };
    conversation_history.push_back(user_message);
    json request_body = {
        {"model", "gpt-4o"},
        {"messages", conversation_history}
    };
    cpr::Response response = cpr::Post(
        cpr::Url{"https://api.nextapi.fun/v1/chat/completions"},
        cpr::Header{{"Authorization", "Bearer " + API_KEY}},
        cpr::Body{request_body.dump()}
    );
    if (response.status_code == 200) {
        auto reply = json::parse(response.text)["choices"][0]["message"]["content"];
        json assistant_reply = {
            {"role", "assistant"},
            {"content", reply}
        };
        conversation_history.push_back(assistant_reply);
        return reply;
    } else {
        std::cerr << "请求失败: " << response.status_code << std::endl;
        return "";
    }
}