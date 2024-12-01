#include <iostream>
#include "../include/third_party/httplib.h" // ��Ҫ���� httplib ��
#include "nlohmann/json.hpp"  // ���ڽ��� JSON �������Ӧ
#include "../include/Server/Server.h"
using json = nlohmann::json;

void test_server() {
    std::cout<<"test1"<<std::endl;
    // ����һ���ͻ��ˣ����ӵ����ط�����
    httplib::Client cli("http://localhost", 3334);

    // ׼����������
    json request_data;
    request_data["session_id"] = "session_1";
    request_data["message"] = "Hello, Server!";

    // ���� POST ���� /chat ·��
    auto res = cli.Post("/chat", request_data.dump(), "application/json");
    std::cout<<"test2"<<std::endl;
    // �жϷ�������Ӧ״̬
    if (res && res->status == 200) {
        std::cout << "Test passed! Response: " << res->body << std::endl;
    } else {
        std::cout << "Test failed. Status code: " << res->status << std::endl;
    }
}

int main() {
    // ����������
    Server server;
    std::thread server_thread([&]() {
        server.start();
    });

    // �ȴ�����������
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // ���в���
    test_server();

    // ֹͣ�����������Ը�����Ҫ�����˳���
    server_thread.detach();

    return 0;
}
