#include "../../include/Agent/Agent.h"
#include "../../include/Model/Model.h"
Agent::Agent() {}
Agent::~Agent() {}

std::vector<Function> Agent::get_tools()
{
    return tools.functions;
}

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

void Agent::insert_memory(const std::string& message, const std::string& role) {
    json user_message = {
        {"role", role},
        {"content", message}
    };
    conversation_history.push_back(user_message);
}

std::string Agent::run(const std::string &message, const bool enable_tools) {
    std::vector<Function> function_tools = enable_tools ? get_tools() : json::parse("[]");
    json messages = model.build_message(
        "gpt-4o",
        "You are a chatbot that can help me with my tasks, you can call tools to help me with my tasks.",
        conversation_history,
        message,
        function_tools
    );
    json answer = model.send_message(messages);
    std::cout<<"answer: "<<answer<<std::endl;

    std::string tool_call = answer["tool_calls"][0]["function"]["name"];
    if (tool_call != "null") {
        std::cout<<"tool_call: "<<tool_call<<std::endl;
        std::string tool_result = tools.handle_tool_call(tool_call, answer["tool_calls"][0]["function"]["arguments"]);
        return tool_result;
    }
    std::cout<<"answer content: "<<answer["content"]<<std::endl;
    return answer["content"] + "\n\n" + EXIT_SIGNAL;
}

std::string Agent::run_until_done(const std::string &message) {
    std::string output;
    int loop_count = 0;
    while (loop_count < MAX_LOOP_COUNT) {
        std::cout<<"loop_count: "<<loop_count<<std::endl;
        output = run(message, loop_count < MAX_LOOP_COUNT - 1);
        if (output.find(EXIT_SIGNAL) != std::string::npos) {
            break;
        }
        loop_count++;
    }
    return output;
}
