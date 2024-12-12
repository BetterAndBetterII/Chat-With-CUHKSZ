#include "../../include/Agent/Agent.h"

Agent::Agent(const std::string& _username, const std::string& _password): tools(new Tools(_username, _password)) {
    this->username = _username;
    this->password = _password;
    this->system_prompt = "Today is " + current_date + ". You are a chatbot that can call tools to help the user with tasks. "
        + "If you or the toolcall result have answered user's question, you must summary what you have done with tools results and add " + EXIT_SIGNAL + " at end of the conversation. "
        + "The username/school number of the user is " + username
        + ". It will be useful when calling email tools."
        + ". The person you are talking to is" + get_name();
}

Agent::~Agent() = default;

std::string Agent::get_current_date() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return std::to_string(1900 + ltm->tm_year) + "-" + std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday);
}

bool Agent::is_valid_login() {
    return tools->is_valid_login(username, password);
}

std::string Agent::get_name() {
    return tools->get_name(username, password);
}

std::vector<Function> Agent::get_tools() const
{
    return tools->functions;
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
    // std::cout<<"answer: "<<answer<<std::endl;
    if (answer["content"].is_null()) {
        // **工具调用**
        std::string tool_call = answer["tool_calls"][0]["function"]["name"];
        json tool_arguments = answer["tool_calls"][0]["function"]["arguments"];
        std::string tool_str = "<Tool Call>: " + tool_call + " with arguments: " + std::string(tool_arguments);
        std::cout<<"tool_str: "<<tool_str<<std::endl;
        insert_memory(tool_str, "assistant");

        std::string tool_result = "<Tool> Called " + tool_str + " <Result>: " + tools->handle_tool_call(tool_call, tool_arguments);
        std::cout<<"tool_result: "<<tool_result<<std::endl;
        insert_memory(tool_result, "user");
        return tool_result;
    }
    // **普通回答**
    insert_memory(answer["content"], "assistant");
    // std::cout<<"answer content: "<<answer["content"]<<std::endl;
    return std::string(answer["content"]) + "\n\n" + EXIT_SIGNAL;
}

void Agent::get_history(json history) {
    conversation_history.insert(conversation_history.end(), history);
}
std::string Agent::run_until_done(const std::string &message) {
    std::string output;
    int loop_count = 0;
    while (loop_count < MAX_LOOP_COUNT) {
        // std::cout<<"loop_count: "<<loop_count<<std::endl;
        output = run(message, loop_count < MAX_LOOP_COUNT - 1);
        insert_memory(output, "user");
        if (output.find(EXIT_SIGNAL) != std::string::npos) {
            break;
        }
        loop_count++;
    }
    return output;
}
