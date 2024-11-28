#include "../../include/Agent/Agent.h"
#include "../../include/Model/Model.h"
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
    Model model;
    std::cout<<message;
    json user_message = {
        {"role", "user"},
        {"content", message}
    };
    conversation_history.push_back(user_message);
    std::string reply=model.send_message(conversation_history);
    json assistant_reply = {
        {"role", "assistant"},
        {"content", reply}
    };
    conversation_history.push_back(assistant_reply);
    return reply;
}