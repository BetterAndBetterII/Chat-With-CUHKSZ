#ifndef SERVER_H
#define SERVER_H

#include <unordered_map>
#include <string>
#include "../../include/Agent/Agent.h"
//#include "../../include/third_party/json.hpp" // nlohmann/json
#include "../../include/third_party/httplib.h"
#include "../../include/History/History.h"

using json = nlohmann::json;

class Server {
public:
    Server();
    ~Server();

    bool login(const std::string& username, const std::string& password);
    void start();
    void handle_post_request(const httplib::Request& req, httplib::Response& res);
    void handle_get_request(const httplib::Request& req, httplib::Response& res);

    std::string handle_message(const std::string& session_id, const std::string& message);
    std::string get_chat_history(const std::string& session_id);
    std::string get_all_first_messages();
private:
    std::unordered_map<std::string, Agent> sessions;
    std::unordered_map<std::string, History> histories;

};

#endif // SERVER_H
