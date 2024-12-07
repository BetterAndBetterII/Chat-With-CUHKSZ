#ifndef CLIENT_H
#define CLIENT_H

#include <string>

using json = nlohmann::json;

class Client {
public:
    Client(const std::string& host, int port);

    bool login(const std::string& username, const std::string& password);
    std::string send_message(const std::string& session_id, const std::string& message);

private:
    httplib::Client http_client_;
};

#endif // CLIENT_H
