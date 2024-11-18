//
// Created by Gary on 24-10-23.
// Complemented by Wen on 24-11-18.
//

#include "../../include/System/Blackboard.h"
#include <iostream> //std::cerr
#include <sstream> 
#include <cstring>
#include <regex> //space_cutter()
#include <chrono> //missDue() 
#include <filesystem> //destructor

BlackBoardSystem::BlackBoardSystem(const std::string& username, const std::string& password) : curl_global_manager(){
    //初始化变量
    this->command_list = {
        "get_course",
        "get_annoucement",
        "get_assignment",
        "get_grades"
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
        std::filesystem::path filepath = cookiefile;  // bbCookies.txt路径
        try {
            if (std::filesystem::remove(filepath)) {
                //std::cout << "CookieFile deleted successfully\n";
            } else {
                std::cout << "CookieFile does not exist or could not be deleted\n";
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

std::vector<std::string> BlackBoardSystem::xpathQuery(const std::string& xmlContent, const std::string& xpathExpr)const {
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
        //std::cout << "No results\n";
        //如果没有匹配正确的内容则返回空的output
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
        cookiefile = username + "bbCookies.txt";
        curl_easy_setopt(bb_handle, CURLOPT_COOKIEJAR,  cookiefile.c_str());  // 保存cookies
        curl_easy_setopt(bb_handle, CURLOPT_COOKIEFILE, cookiefile.c_str()); // 发送保存的cookies

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

    std::cerr << "Failed to initialize bb_handle." << std::endl;
    return false;

}

std::string BlackBoardSystem::getRequest(const std::string& url)const{
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

std::string BlackBoardSystem::postRequest(const std::string& url, const std::string& strdata)const{
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


std::string BlackBoardSystem::get_commands()const{

    std::ostringstream result;
    for(auto it = command_list.begin(); it != command_list.end(); ++it ){
        result << *it;
        if(std::next(it) != command_list.end()){
            result << ", ";
        }
    }
    return result.str();
}


std::string BlackBoardSystem::get_course(const std::string& term)const{
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

std::string BlackBoardSystem::get_course_id(const std::string& crouse)const{
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

std::string BlackBoardSystem::get_announcement(const std::string& crouse, const int number )const{
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

std::string BlackBoardSystem::get_assignment(const std::string& crouse)const{
    std::string rawData = getRequest("https://bb.cuhk.edu.cn/webapps/bb-enhance-BBLEARN/normal/mycourse/search"); 
    std::vector<std::string> crouse_name = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[1]/span[2]");
    std::vector<std::string> crouse_id = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/th/a/@onclick");
    //截取id部分
    std::for_each(crouse_id.begin(), crouse_id.end(), [](std::string& str) {
        str = str.substr(str.find("id=") + 3, str.find("&url") - str.find("id=")-3);
    }); 

    std::vector<std::string> total_assignment_urls;

    for(int i = 0 ; i < crouse_id.size(); ++i){
        if((crouse_name[i].find(crouse) != std::string::npos)||(crouse=="ALL")){
            std::string crouseData = getRequest("https://bb.cuhk.edu.cn/webapps/blackboard/execute/launcher?type=Course&id=" + crouse_id[i] + "&url=");
            std::vector<std::string> content_urls = xpathQuery(crouseData, "//a[starts-with(@href, '/webapps/blackboard/content/listContent')]/@href");
            for(std::string contetn_url : content_urls){
                recursive_search_assignments(
                    getRequest("https://bb.cuhk.edu.cn"+contetn_url),
                    total_assignment_urls
                );
            }
        }
    }

    std::stringstream result;
    result << crouse << " assignments:" << std::endl;
    for(std::string assignment_url : total_assignment_urls){
        result << parse_assignment_url("https://bb.cuhk.edu.cn"+assignment_url) << std::endl;
    }

    return result.str();
}


void BlackBoardSystem::recursive_search_assignments(const std::string& data, std::vector<std::string>& total_assignmnet_urls)const{

    std::vector<std::string> assignment_urls = xpathQuery(data, "//a[starts-with(@href, '/webapps/assignment/uploadAssignment') or starts-with(@href, '/webapps/blackboard/content/launchAssessment') ]/@href");
    std::vector<std::string> content_urls = xpathQuery(data, "/html/body/div/div/div/div/div/div/div/ul/li/div/h3/a[starts-with(@href, '/webapps/blackboard/content/listContent')]/@href");

    for(std::string assignmnet_url : assignment_urls){
        total_assignmnet_urls.push_back(assignmnet_url);
    }

    if(!content_urls.empty()){
        for(std::string content_url : content_urls){
        
            //std::cout << "https://bb.cuhk.edu.cn" + content_url << std::endl;
            recursive_search_assignments(
                getRequest("https://bb.cuhk.edu.cn" + content_url),
                total_assignmnet_urls
            );
        }
    }
}

std::string BlackBoardSystem::vector_toString(const std::vector<std::string>& vector)const{
    std::string result;
    for(std::string element : vector){
        result+=element+"\n";
    }
    return result;
}

std::string BlackBoardSystem::space_cutter(const std::string& str)const{
    std::regex pattern("^\\s*(.*?)\\s*$");
    std::smatch match;
    if (std::regex_match(str, match, pattern)) {
        return match[1];
    } else {
        return "";
    }
}

std::string BlackBoardSystem::parse_assignment_url(const std::string& url)const{
    std::stringstream result;
    std::string rawData = getRequest(url);
    if(url.find("launchAssessment")!=std::string::npos){
        std::string header = space_cutter(xpathQuery(rawData, "//span[@id='pageTitleText']")[0]);
        std::string title = header.substr(header.find(":")+1);
        std::string dueinfo = space_cutter(xpathQuery(rawData, "//*[@id='stepcontent1']/ol/li[position()=last()]/div[2]/text()")[0]);

        result << title << std::endl;

        //提取时间
        int startpos = dueinfo.find("due on ")+7;
        std::string duedate = dueinfo.substr(startpos);
        int endpos = duedate.find("Test");
        duedate = duedate.substr(0, endpos);
        result << "Duedate:" << duedate <<std::endl;
        result << "you need visit bb.cuhk.edu.cn to check wether this assignment is done." << std::endl;

    }
    else{
        std::vector<std::string> header = xpathQuery(rawData, "//li[@class='placeholder']/span");
        std::string status = header[0].substr(0,header[0].find(":"));
        std::string title = header[0].substr(header[0].find(":")+2);
        title = space_cutter(title);
        result << title << std::endl;
        if(status.find("Review")!=std::string::npos){
            result << "Status: Already Upload!" << std::endl;
            std::vector<std::string> grades = xpathQuery(rawData, "//*[@id='aggregateGrade']/@value");
            std::vector<std::string> maximum_grades = xpathQuery(rawData, "//*[@id='aggregateGrade_pointsPossible']");
            result << grades[0] << maximum_grades[0] << std::endl;
            if(grades[0].find("-")!=std::string::npos){
                result << "Grades not yet updated" << std::endl;
            }
        }
        if(status.find("Upload")!=std::string::npos){
            std::string due = space_cutter(xpathQuery(rawData, "//*[@id='metadata']/div/div/div[1]/div[2]/text()")[0])
                            + space_cutter(xpathQuery(rawData, "//*[@id='metadata']/div/div/div[1]/div[2]/span")[0]);
            std::string possible_points = space_cutter(xpathQuery(rawData, "//*[@id='metadata']/div/div/div[2]/div[2]/text()")[0]);
            result << "Points Possible: " << possible_points << std::endl;
            result << "DueDate: " << due << std::endl;
            if (missDue(due, "%A, %B %d, %Y %I:%M %p" )) {
                result << "Missed the due date!" << std::endl;
            }
            else{
                result << "Need Upload!" << std::endl;
            }
        }
    }

    return result.str();

}


bool BlackBoardSystem::missDue(const std::string& content, const std::string& pattern)const{

    // 定义时间结构体
    std::tm tm = {};
    std::istringstream ss(content);

    // 解析时间字符串（需根据实际格式调整格式符）
    ss >> std::get_time(&tm, pattern.c_str());
    if (ss.fail()) {
        std::cerr << "Failed to parse time string!" << std::endl;
    }

    // 将 tm 转换为 time_t
    std::time_t input_time_t = std::mktime(&tm);
    if (input_time_t == -1) {
        std::cerr << "Failed to convert to time_t!" << std::endl;
    }
    auto now_time = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now_time);
    if (input_time_t <= now_time_t) {
        return true;
    }
    
    return false;
    
}

std::string BlackBoardSystem::get_grades(const std::string& crouse)const{
    std::string id = get_course_id(crouse);
    if(!id.empty()){
        std::string url ="https://bb.cuhk.edu.cn/webapps/bb-mygrades-BBLEARN/myGrades?course_id=" + id + "&stream_name=mygrades";
        std::string rawData = getRequest(url);
        std::vector<std::string> name = xpathQuery(rawData, "//div[@id='grades_wrapper']/div/div[@class='cell gradable']/span | //div[@id='grades_wrapper']/div/div[@class='cell gradable']/a ");
        std::vector<std::string> grades = xpathQuery(rawData, "//div[@id='grades_wrapper']/div/div[@class='cell grade']/span[1]");
        std::vector<std::string> grade_time = xpathQuery(rawData, "//div[@id='grades_wrapper']/div/div[@class='cell activity timestamp']/span[1]");
        std::vector<std::string> grade_stamp = xpathQuery(rawData, "//div[@id='grades_wrapper']/div/div[@class='cell activity timestamp']/span[2]");
        

        std::stringstream total_result;
        total_result << crouse << " grades: " << std::endl;
        for(int i = 0 ; i < name.size(); ++i){
            total_result << space_cutter(name[i]) + " " + space_cutter(grade_stamp[i]) << std::endl;
            total_result << "Grades: " << space_cutter(grades[i]) << std::endl;
            total_result << space_cutter(grade_time[i]) << std::endl;
        }

        return total_result.str();
    }
    return "";
}