//
// Created by Gary on 24-10-23.
//
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <filesystem>
#include <curl/curl.h>
#include <set> //std::set<> command_list
#include <vector>
#include <algorithm>
#include "../../include/System/Email.h"
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

EmailSystem::EmailSystem(const std::string& username, const std::string& password) : curl_global_manager(){
    //初始化变量
    this->command_list = {
        "send_email"
    };
    this->username = username;
    this->password = password;
    this->email_handle = curl_easy_init();
    this->is_login = false;

}

EmailSystem::~EmailSystem(){
    if(email_handle){
        curl_easy_cleanup(email_handle);
        //delete bbcookie.txt
        std::filesystem::path filepath = "emailCookies.txt";  // bbCookies.txt路径
        try {
            if (std::filesystem::remove(filepath)) {
                //std::cout << "CookieFile deleted successfully\n";
            } else {
                //std::cout << "CookieFile does not exist or could not be deleted\n";
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
        //调试：检测email_handle是否清除
        //std::cout << "email_handle cleaned up" << std::endl;
    }
}

//callback functions implementation
size_t EmailSystem::ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    return size * nmemb;  // 只是返回数据大小，忽略响应体
}

size_t EmailSystem::write_callback(char *data, size_t size, size_t nmemb, void *clientp)
{
  size_t realsize = size * nmemb;
  struct Memory *mem =(struct Memory *)clientp;

  char *ptr = (char*)realloc(mem->response, mem->size + realsize + 1);
  if(!ptr)
    return 0;  /* out of memory */

  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;

  return realsize;
}

std::vector<std::string> EmailSystem::xpathQuery(const std::string& xmlContent, const std::string& xpathExpr) {
    std::vector<std::string> output;
    // 将字符串形式的XML内容解析为libxml2文档对象
    xmlDocPtr doc = htmlReadMemory(xmlContent.c_str(), xmlContent.size(), NULL, "utf-8", HTML_PARSE_RECOVER|HTML_PARSE_NOERROR);
    if (doc == nullptr) {
        std::cerr << "Failed to parse document\n";
        return output;
    }

    // 创建XPath上下文，用于在该文档中执行XPath查询
    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    if (context == nullptr) {
        std::cerr << "Failed to create XPath context\n";
        xmlFreeDoc(doc);
        return output;
    }

    // 使用XPath表达式在文档中查找匹配的节点
    xmlXPathObjectPtr result = xmlXPathEvalExpression((const xmlChar*)xpathExpr.c_str(), context);
    if (result == nullptr) {
        std::cerr << "Failed to evaluate XPath expression\n";
        xmlXPathFreeContext(context);
        xmlFreeDoc(doc);
        return output;
    }

    if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        std::cout << "No results\n";
    } else {
        // 遍历找到的节点集合，获取每个节点的内容
        xmlNodeSetPtr nodes = result->nodesetval;
        for (int i = 0; i < nodes->nodeNr; i++) {
            xmlNodePtr node = nodes->nodeTab[i];
            xmlChar* content = xmlNodeGetContent(node);
            output.push_back(reinterpret_cast<char*>(content));
            xmlFree(content);
        }
    }

    // 释放XPath对象和上下文资源
    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    return output;
}

//login function implementation
bool EmailSystem::login(){
    if(is_login){
        return true;
    }

    //尝试登录
    if(email_handle){
        //忽略登录过程返回的响应体（注释下行可把响应体打印到终端）
        curl_easy_setopt(email_handle, CURLOPT_WRITEFUNCTION, ignore_calback);
        const std::string str_url = std::string(SMTP_SERVER) + ":" + std::to_string(SMTP_PORT);
        const char* smtp_url = str_url.c_str();
        curl_easy_setopt(email_handle, CURLOPT_URL, smtp_url);

        //设置用户名和密码
        curl_easy_setopt(email_handle, CURLOPT_USERNAME, (username + std::string(EMAIL_POSTFIX)).c_str());
        curl_easy_setopt(email_handle, CURLOPT_PASSWORD, password.c_str());

        //TLS加密
        curl_easy_setopt(email_handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        // 发起空请求，验证是否可以登录
        curl_easy_setopt(email_handle, CURLOPT_MAIL_FROM, "<>");
        curl_slist* recipients = nullptr;
        recipients = curl_slist_append(recipients, "<>");
        curl_easy_setopt(email_handle, CURLOPT_MAIL_RCPT, recipients);

        CURLcode res = curl_easy_perform(email_handle);

        if(res != CURLE_OK){
            std::cerr << "Login failed because:" << curl_easy_strerror(res) << std::endl;
            return false;
        }

        //销毁收件人列表，连接
        if (recipients) {
            curl_slist_free_all(recipients);
        }

        //登录成功
        is_login = true;
        return true;
    }

    throw std::runtime_error("Failed to initialize bb_handle.");
}

bool EmailSystem::change_info(const std::string& username, const std::string& password){
    if(is_login){
        return false;
    }
    this->username = username;
    this->password = password;
    return true;
}


std::string EmailSystem::get_commands(){

    std::ostringstream result;
    for(auto it = command_list.begin(); it != command_list.end(); ++it ){
        result << *it;
        if(std::next(it) != command_list.end()){
            result << ", ";
        }
    }
    return result.str();
}

std::string EmailSystem::create_message(const std::vector<std::string>& recipients_str, const std::string& subject, const std::string& body) const
{
    std::string email = "From: " + username + std::string(EMAIL_POSTFIX) + "\r\n";
    email += "To: ";
    for (size_t i = 0; i < recipients_str.size(); ++i) {
        email += recipients_str[i];
        if (i != recipients_str.size() - 1) {
            email += ", ";
        }
    }
    email += "\r\n";
    email += "Subject: " + subject + "\r\n";
    email += "MIME-Version: 1.0\r\n";
    email += "Content-Type: text/plain; charset=utf-8\r\n";
    email += "Content-Transfer-Encoding: 8bit\r\n";
    email += "Message-ID: <" + std::to_string(time(nullptr)) + username + std::string(EMAIL_POSTFIX) + ">\r\n";
    email += "\r\n";
    // body中可以包含html标记，如 <h1>标题</h1><p>这是正文</p>
    email += body;
    return email;
}

size_t EmailSystem::payload_source(void* ptr, size_t size, size_t nmemb, void* userp) {
    auto* upload_ctx = static_cast<UploadStatus*>(userp);
    const char* data = upload_ctx->system->email_content.c_str() + upload_ctx->bytes_read;
    size_t room = size * nmemb;
    size_t len = upload_ctx->system->email_content.size() - upload_ctx->bytes_read;

    if (len > 0) {
        size_t copy_size = (len < room) ? len : room;
        memcpy(ptr, data, copy_size);
        upload_ctx->bytes_read += copy_size;
        return copy_size;
    }

    return 0; // No more data
}

std::string EmailSystem::send_email(const std::vector<std::string>& recipients_str, const std::string& subject, const std::string& body) {
    if (!login()) {
        return "Login failed.";
    }

    // 重置所有选项
    curl_easy_reset(email_handle);

    // 设置 SMTP 服务器
    const std::string str_url = std::string(SMTP_SERVER) + ":" + std::to_string(SMTP_PORT);
    curl_easy_setopt(email_handle, CURLOPT_URL, str_url.c_str());

    // 初始设置 - 不立即使用 SSL/TLS
    curl_easy_setopt(email_handle, CURLOPT_USE_SSL, CURLUSESSL_NONE);
    
    // 设置为使用 STARTTLS
    curl_easy_setopt(email_handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    // 明确指示使用 STARTTLS
    curl_easy_setopt(email_handle, CURLOPT_CUSTOMREQUEST, "STARTTLS");

    // 设置登录凭据
    curl_easy_setopt(email_handle, CURLOPT_USERNAME, (username + std::string(EMAIL_POSTFIX)).c_str());
    curl_easy_setopt(email_handle, CURLOPT_PASSWORD, password.c_str());

    // 设置发件人和收件人
    curl_easy_setopt(email_handle, CURLOPT_MAIL_FROM, ("<" + username + std::string(EMAIL_POSTFIX) + ">").c_str());

    curl_slist* recipients = nullptr;
    for (const auto& recipient : recipients_str) {
        recipients = curl_slist_append(recipients, ("<" + recipient + ">").c_str());
    }
    curl_easy_setopt(email_handle, CURLOPT_MAIL_RCPT, recipients);

    // 准备邮件内容
    email_content = create_message(recipients_str, subject, body);
    UploadStatus upload_ctx(this);

    // 设置上传回调
    curl_easy_setopt(email_handle, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(email_handle, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(email_handle, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(email_handle, CURLOPT_INFILESIZE, email_content.size());

    // 设置超时
    curl_easy_setopt(email_handle, CURLOPT_TIMEOUT, 60L);

    // 发送邮件
    CURLcode res = curl_easy_perform(email_handle);
    
    // 释放收件人列表
    if (recipients) {
        curl_slist_free_all(recipients);
    }

    if (res != CURLE_OK) {
        return "Send email failed because: " + std::string(curl_easy_strerror(res));
    }

    return "Send email success.";
}
