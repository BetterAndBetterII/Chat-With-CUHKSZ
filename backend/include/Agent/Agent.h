#ifndef AGENT_H
#define AGENT_H
#include <string>
#include <nlohmann/json.hpp>
#include "../System/Tools.h"
#include "../Model/Model.h"

using json = nlohmann::json;
class Agent {
private:
    int MAX_LOOP_COUNT = 10;
    std::string EXIT_SIGNAL = "<exit>";
    std::vector<json> conversation_history;
    std::string current_date = get_current_date();
    std::string username;
    std::string password;
    std::string system_prompt;
    Model model;
    Tools* tools;
public:
	Agent() = default;
    explicit Agent(const std::string& _username, const std::string& _password);
    ~Agent();

    [[nodiscard]] std::vector<Function> get_tools() const;

    void insert_memory(const std::string& message, const std::string& role);

    std::string run(const std::string &message, bool enable_tools);

    void get_history(json history);

    std::string run_until_done(const std::string &message);

    static std::string get_current_date();

    bool is_valid_login();

};
#endif