#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "../../include/third_party/httplib.h"
using json = nlohmann::json;

class Client {
public:
    Client(const std::string& host, int port);

    bool login(const std::string& username, const std::string& password);
    std::string send_message(const std::string& session_id, const std::string& message);
    std::string get_chat_history(const std::string& session_id);
    std::string get_first_messages();

private:
    httplib::Client http_client_;
};

#endif // CLIENT_H
