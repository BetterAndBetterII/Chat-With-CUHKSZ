#include "third_party/httplib.h"
#include <iostream>

int main() {
    httplib::Server svr;

    svr.Get("/helloworld", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("HelloWorld", "text/plain");
        });
    svr.Get("/hellolwt", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("HelloLwt", "text/plain");
        });
    std::cout << "Starting server..." << std::endl;
    if (!svr.listen("0.0.0.0", 8080)) {
        std::cerr << "Error: Unable to start the server. Port may be in use." << std::endl;
        return 1;
    }

    svr.listen("0.0.0.0", 8080);  // 如果无法监听端口，程序可能会静默退出

    std::cout << "Server stopped." << std::endl;  // 这行只有在服务器被手动停止时才会输出

    return 0;
}
