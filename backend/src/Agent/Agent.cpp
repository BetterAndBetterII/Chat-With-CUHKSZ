#include "../../include/Agent/Agent.h"
#include "../../include/Model/Model.h"
Agent::Agent() = default;
Agent::~Agent() = default;

std::vector<Function> Agent::get_tools() const
{
    return tools.functions;
}

void Agent::insert_memory(const std::string& message, const std::string& role) {
    json user_message = {
        {"role", role},
        {"content", message}
    };
    conversation_history.insert(conversation_history.end(), user_message);
}

std::string Agent::run(const std::string &message, const bool enable_tools) {
    std::vector<Function> function_tools = enable_tools ? get_tools() : std::vector<Function>();
    json messages = model.build_message(
        "gpt-4o",
        system_prompt,
        conversation_history,
        message,
        function_tools
    );
    json answer = model.send_message(messages);
    std::cout<<"answer: "<<answer<<std::endl;
    if (answer["content"].is_null()) {
        // **工具调用**
        std::string tool_call = answer["tool_calls"][0]["function"]["name"];
        json tool_arguments = answer["tool_calls"][0]["function"]["arguments"];
        std::string tool_str = "<Tool Call>: " + tool_call + " with arguments: " + std::string(tool_arguments);
        insert_memory(tool_str, "assistant");

        std::cout<<tool_str<<std::endl;
        std::string tool_result = "<Tool> Called " + tool_str + " <Result>: " + Tools::handle_tool_call(tool_call, tool_arguments);
        insert_memory(tool_result, "user");
        return tool_result;
    }
    // **普通回答**
    insert_memory(answer["content"], "assistant");
    std::cout<<"answer content: "<<answer["content"]<<std::endl;
    return std::string(answer["content"]) + "\n\n" + EXIT_SIGNAL;
}

std::string Agent::run_until_done(const std::string &message) {
    std::string output;
    int loop_count = 0;
    while (loop_count < MAX_LOOP_COUNT) {
        std::cout<<"loop_count: "<<loop_count<<std::endl;
        output = run(message, loop_count < MAX_LOOP_COUNT - 1);
        insert_memory(output, "user");
        if (output.find(EXIT_SIGNAL) != std::string::npos) {
            break;
        }
        loop_count++;
    }
    return output;
}
