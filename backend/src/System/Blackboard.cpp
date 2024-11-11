//
// Created by Gary on 24-10-23.
//
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <curl/curl.h>
#include <set> //std::set<> command_list
#include "../../include/System/Blackboard.h"

BlackBoardSystem::BlackBoardSystem(const std::string& username, const std::string& password) : curl_global_manager(){
    //初始化变量
    this->command_list = {
        "command0",
        "command1", 
        "command2"
    };
    this->username = username;
    this->password = password;
    this->bb_handle = curl_easy_init();
    this->is_login = false;

}

BlackBoardSystem::~BlackBoardSystem(){
    if(bb_handle){
        curl_easy_cleanup(bb_handle);
        //delete bbcookie.txt
        std::filesystem::path filepath = "bbCookies.txt";  // bbCookies.txt路径
        try {
            if (std::filesystem::remove(filepath)) {
                //std::cout << "CookieFile deleted successfully\n";
            } else {
                //std::cout << "CookieFile does not exist or could not be deleted\n";
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cout << "Error: " << e.what() << "\n";
        } 
        //调试：检测bb_handle是否清除
        //std::cout << "bb_handle cleaned up" << std::endl;
    }
}

//callback functions implementation
size_t BlackBoardSystem::ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    return size * nmemb;  // 只是返回数据大小，忽略响应体
}

//login function implementation
bool BlackBoardSystem::login(){
    if(is_login){
        return true;
    }

    //尝试登录
    if(bb_handle){

        //忽略登录过程返回的响应体（注释下行可把响应体打印到终端）
        curl_easy_setopt(bb_handle, CURLOPT_WRITEFUNCTION, ignore_calback);

        // 启用自动cookie处理，指定cookie文件
        curl_easy_setopt(bb_handle, CURLOPT_COOKIEJAR,  "bbCookies.txt");  // 保存cookies
        curl_easy_setopt(bb_handle, CURLOPT_COOKIEFILE,  "bbCookies.txt"); // 发送保存的cookies

        //向sts.cuhk.edu.cn发送登录请求(POST)
        std::string strdata ="UserName=cuhksz\\" + username + "&Kmsi=true&AuthMethod=FormsAuthentication&Password=" + password ; //POST data
        const char* data = strdata.c_str();
        curl_easy_setopt(bb_handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(bb_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(bb_handle, CURLOPT_URL, "https://sts.cuhk.edu.cn/adfs/oauth2/authorize?response_type=code&client_id=4b71b947-7b0d-4611-b47e-0ec37aabfd5e&redirect_uri=https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode&client-request-id=dd5ffbba-e761-453e-e58f-014001000089");
        CURLcode res;
        res = curl_easy_perform(bb_handle);
        
        //通过重定向次数判断是否成功登录
        long redirect_count;
        curl_easy_getinfo(bb_handle, CURLINFO_REDIRECT_COUNT, &redirect_count);
        if(res != CURLE_OK){
            std::cout << "Login failed because:" << curl_easy_strerror(res) << std::endl;
        }
        else if(redirect_count > 0 ){
            is_login = true;
            std::cout << "Login successfully!" << std::endl;
            return true;
        }
        else{
            std::cout << "Username or Password incorrect!" << std::endl;
            return false;
        }

    }

    throw std::runtime_error("Failed to initialize bb_handle.");
    return false;

}

std::string BlackBoardSystem::execute_command(const std::string& command) {
    //return iterator of "command"
    auto it = command_list.find(command);
    if(it != command_list.end()){
        //TODO:excute command
        //尝试POST获取annoucement
        FILE *output = fopen("annoucement.html", "w");
        curl_easy_setopt(bb_handle, CURLOPT_URL, "https://bb.cuhk.edu.cn/webapps/blackboard/execute/announcement?method=search&context=mybb&handle=my_announcements&returnUrl=/webapps/portal/execute/tabs/tabAction?tab_tab_group_id=_1_1&tabId=_1_1&forwardUrl=index.jsp");
        curl_easy_setopt(bb_handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(bb_handle, CURLOPT_HEADER, 0L); 
        curl_easy_setopt(bb_handle, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(bb_handle, CURLOPT_WRITEDATA, output);
        CURLcode res;
        res = curl_easy_perform(bb_handle);  //post请求anouncement网页
        fclose(output);
        return "excute " + command;
    }
    return "Command Not Found!";
}

std::string BlackBoardSystem::show_commands(){
    std::ostringstream result;
    for(auto it = command_list.begin(); it != command_list.end(); ++it ){
        result << *it;
        if(std::next(it) != command_list.end()){
            result << ", ";
        }
    }
    return result.str();
}

