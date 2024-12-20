#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <nlohmann/json.hpp>
#include "../../include/third_party/httplib.h"
using json = nlohmann::json;

class Client {
public:
    Client(const std::string& host, int port);

    bool login(const std::string& username, const std::string& password);
    std::string send_message( const std::string& session_id, const std::string& message);
    std::string get_chat_history(const std::string& session_id);
    std::string get_first_messages();
    bool test_connection() const;

private:
    httplib::Client http_client_;
    std::string host;
    int port;
    std::string username;
    std::string bind_to_username(const std::string& session_id);

};

#endif // CLIENT_H
