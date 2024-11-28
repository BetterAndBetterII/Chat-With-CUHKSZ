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
#include "../../include/System/Blackboard.h"
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h> 

BlackBoardSystem::BlackBoardSystem(const std::string& username, const std::string& password) : curl_global_manager(){
    //初始化变量
    this->command_list = {
        "get_course",
        "get_annoucement"
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
            std::cerr << "Error: " << e.what() << "\n";
        } 
        //调试：检测bb_handle是否清除
        //std::cout << "bb_handle cleaned up" << std::endl;
    }
}

//callback functions implementation
size_t BlackBoardSystem::ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    return size * nmemb;  // 只是返回数据大小，忽略响应体
}

size_t BlackBoardSystem::write_callback(char *data, size_t size, size_t nmemb, void *clientp)
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

std::vector<std::string> BlackBoardSystem::xpathQuery(const std::string& xmlContent, const std::string& xpathExpr) {
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
        std::cout<<username<<"   "<<password<<std::endl;
        if(res != CURLE_OK){
            std::cerr << "Login failed because:" << curl_easy_strerror(res) << std::endl;
        }
        else if(redirect_count > 0 ){
            is_login = true;
            std::cout << "Login successfully!" << std::endl;
            return true;
        }
        else{
            std::cerr << "Username or Password incorrect!" << std::endl;
            return false;
        }

    }

    throw std::runtime_error("Failed to initialize bb_handle.");
    return false;

}

std::string BlackBoardSystem::getRequest(const std::string& url){
    std::string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(bb_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(bb_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(bb_handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(bb_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(bb_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        CURLcode res;
        res = curl_easy_perform(bb_handle);  
        if(res != CURLE_OK){
            std::cerr<< "Get request failed:" << curl_easy_strerror(res) << std::endl;
        }

        if (chunk.response){
            response = chunk.response;
            free(chunk.response);
        }
        else{
            std::cerr << "No response received from server.\n";
        }

        return response; 
    }
    std::cerr<< "GetRequest failed:Login before get request" << std::endl;
    return response;
}

std::string BlackBoardSystem::postRequest(const std::string& url, const std::string& strdata){
    std::string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(bb_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(bb_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(bb_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        const char* data = strdata.c_str();
        curl_easy_setopt(bb_handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(bb_handle, CURLOPT_FOLLOWLOCATION, 1L);
        CURLcode res;
        res = curl_easy_perform(bb_handle);  
        if(res != CURLE_OK){
            std::cerr<< "Post request failed:" << curl_easy_strerror(res) << std::endl;
        }

        if (chunk.response){
            response = chunk.response;
            free(chunk.response);
        }
        else{
            std::cerr << "No response received from server.\n";
        }

        return response; 
    }
    std::cerr<< "PostRequest failed:Login before post request" << std::endl;
    return response;
}

bool BlackBoardSystem::change_info(const std::string& username, const std::string& password){
    if(is_login){
        return false;
    }
    this->username = username;
    this->password = password;
    return true;
}


std::string BlackBoardSystem::get_commands(){

    std::ostringstream result;
    for(auto it = command_list.begin(); it != command_list.end(); ++it ){
        result << *it;
        if(std::next(it) != command_list.end()){
            result << ", ";
        }
    }
    return result.str();
}

std::string BlackBoardSystem::execute_command(const std::string& command) {
    //return iterator of "command"
    auto it = command_list.find(command);
    if(it != command_list.end()){
        //excute command
        if(*it == "get_course"){return get_course();}
    }
    return "Command Not Found!";
}

std::string BlackBoardSystem::get_course(const std::string& term){
    change_info(username, password);
    if (!login()) {
        return "Invalid username or password!";
    }
    std::string rawData = getRequest("https://bb.cuhk.edu.cn/webapps/bb-enhance-BBLEARN/normal/mycourse/search"); 
    std::vector<std::string> crouse_name = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[1]/span[2]");
    std::vector<std::string> crouse_instructor = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[3]/span[2]");
    std::vector<std::string> crouse_term = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[4]/span[2]");

    std::string total_result = "";
    for(int i = 0 ; i < crouse_name.size(); ++i){
        if(crouse_term[i] == term ){
            total_result+= "Crouse Name: " + crouse_name[i] + "\n";
            total_result+= "Instructor: " + crouse_instructor[i] + "\n";
            total_result+= "Crouse Term: " + crouse_term[i] + "\n\n";
        }
    }

    return total_result;
}

std::string BlackBoardSystem::get_course_id(const std::string& crouse){
    std::string rawData = getRequest("https://bb.cuhk.edu.cn/webapps/bb-enhance-BBLEARN/normal/mycourse/search"); 
    std::vector<std::string> crouse_name = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[1]/span[2]");
    std::vector<std::string> crouse_id = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/th/a/@onclick");
    //截取id部分
    std::for_each(crouse_id.begin(), crouse_id.end(), [](std::string& str) {
        str = str.substr(str.find("id=") + 3, str.find("&url") - str.find("id=")-3);
    }); 
    for(int i = 0 ; i < crouse_id.size(); ++i){
        if(crouse_name[i].find(crouse) != std::string::npos ){
            return crouse_id[i];
        }
    }
    return "";
}

std::string BlackBoardSystem::get_announcement(const std::string& crouse, const int number ){
    std::string id = get_course_id(crouse);
    if(!id.empty()){
        std::string data ="method=search&viewChoice=3&editMode=false&tabAction=false&announcementId=&course_id=&context=mybb&internalHandle=my_announcements&searchSelect=" + id; //POST data
        std::string rawData = postRequest("https://bb.cuhk.edu.cn/webapps/blackboard/execute/announcement", data);
        std::vector<std::string> headers = xpathQuery(rawData, "//li[@class='clearfix']/h3");
        std::vector<std::string> postTime = xpathQuery(rawData, "//li[@class='clearfix']/div[@class='details']/p[1]");
        std::vector<std::string> details = xpathQuery(rawData, "//li[@class='clearfix']/div[@class='details']/div");
        std::vector<std::string> posters = xpathQuery(rawData, "//li[@class='clearfix']/div[@class='announcementInfo']/p[1]");
        
        //去除多余空格
        std::for_each(headers.begin(), headers.end(), [](std::string& str) {
            str = str.substr(str.find_first_not_of(" \t\n\r\v"));
        }); 

        std::string total_result = "";
        for(int i = 0 ; i < headers.size() && i < number; ++i){
            total_result+="Announcement:\n"+headers[i]+"\n"+postTime[i]+"\n"+details[i]+"\n"+posters[i]+"\n";
        }

        return total_result;
    }
    return "";
}

std::string BlackBoardSystem::get_undo_assignment(){
    std::string rawData = getRequest("https://bb.cuhk.edu.cn/webapps/bb-enhance-BBLEARN/normal/mycourse/search"); 
    std::vector<std::string> crouse_name = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[1]/span[2]");
    std::vector<std::string> crouse_id = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/th/a/@onclick");
    //截取id部分
    std::for_each(crouse_id.begin(), crouse_id.end(), [](std::string& str) {
        str = str.substr(str.find("id=") + 3, str.find("&url") - str.find("id=")-3);
    }); 
    //遍历查找作业
    std::string undo_assignments = "";
    for(int i = 0 ; i < crouse_id.size() ; ++i){
        std::string crouse_content = getRequest("https://bb.cuhk.edu.cn/webapps/blackboard/execute/launcher?type=Course&id="+crouse_id[i]+"&url=");
        std::vector<std::string> undo = xpathQuery(crouse_content, "/html/body/div[5]/div[3]/div/div[1]/div/div/div[2]/div[2]/div[1]/div[2]/div/div/div[3]/div[3]/div/ul/li[4]");
        if(!undo.empty()){
            std::cout << crouse_name[i] << std::endl;
            std::cout << undo[0] << std::endl;
        }

    }

    return undo_assignments;
}