//
// Created by Wen on 24-11-27.
//
#include "../../include/System/Sis.h"
#include <iostream> //std::cerr
#include <sstream> 
#include <cstring>
#include <regex> //space_cutter()
#include <chrono> //missDue() 
#include <filesystem> //destructor
#include <random>

using std::string;
using std::vector;
using std::cout;
using std::endl;

SisSystem::SisSystem(const string& username, const string& password) : curl_global_manager(){
    //初始化变量
    this->command_list = {
        "get_course",
        "get_annoucement",
        "get_assignment",
        "get_grades"
    };
    this->username = username;
    this->password = password;
    this->sis_handle = curl_easy_init();
    this->is_login = false;

}

SisSystem::~SisSystem(){
    if(sis_handle){
        curl_easy_cleanup(sis_handle);
        //delete siscookie.txt
            std::filesystem::path filepath = cookiefile;  // bbCookies.txt路径
            try {
            if (std::filesystem::remove(filepath)) {
                //cout << "CookieFile deleted successfully\n";
            } else {
                cout << "CookieFile does not exist or could not be deleted\n";
            }
            } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << "\n";
            }  
        //调试：检测sis_handle是否清除
        //cout << "sis_handle cleaned up" << endl;
    }
}

//callback functions implementation
size_t SisSystem::ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    return size * nmemb;  // 只是返回数据大小，忽略响应体
}

size_t SisSystem::write_callback(char *data, size_t size, size_t nmemb, void *clientp)
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

size_t SisSystem::header_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    // 计算头部的大小
    size_t header_size = size * nmemb;
    // 打印响应头
    cout << "---Response Header----" << endl;
    printf("%.*s", (int)header_size, ptr);
    return header_size; // 返回处理的字节数
}

size_t SisSystem::debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr) {
    (void)handle;  // 如果不需要 handle，可以忽略
    (void)userptr; // 如果不需要 userptr，可以忽略

    switch (type) {
        case CURLINFO_TEXT:
            //fprintf(stderr, "== Info: %.*s", (int)size, data);
            break;
        case CURLINFO_HEADER_OUT:
            fprintf(stderr, ">> Request Header: %.*s", (int)size, data);
            break;
        case CURLINFO_DATA_OUT:
            fprintf(stderr, ">> Request Body: %.*s", (int)size, data);
            break;
        case CURLINFO_HEADER_IN:
            fprintf(stderr, "<< Response Header: %.*s", (int)size, data);
            break;
        case CURLINFO_DATA_IN:
            //fprintf(stderr, "<< Response Body: %.*s", (int)size, data);
            break;
        default:
            break;
    }
    return 0;
}

vector<string> SisSystem::xpathQuery(const string& xmlContent, const string& xpathExpr)const {
    vector<string> output;
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
        //cout << "No results\n";
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
bool SisSystem::login(){
    if(is_login){
        return true;
    }

    //尝试登录
    if(sis_handle){

        CURLcode res;
        //忽略登录过程返回的响应体（注释下行可把响应体打印到终端）
        curl_easy_setopt(sis_handle, CURLOPT_WRITEFUNCTION, ignore_calback);
        //debug print info
        curl_easy_setopt(sis_handle, CURLOPT_VERBOSE, 1L);//详细输出
        curl_easy_setopt(sis_handle, CURLOPT_DEBUGFUNCTION, debug_callback);

        // 启用自动cookie处理，指定cookie文件
        cookiefile = username + "sisCookies.txt";
        curl_easy_setopt(sis_handle, CURLOPT_COOKIEJAR,  cookiefile.c_str());  // 保存cookies
        curl_easy_setopt(sis_handle, CURLOPT_COOKIEFILE, cookiefile.c_str()); // 发送保存的cookies
        // 自定义 HTTP 请求头
        struct curl_slist* headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36");
        headers = curl_slist_append(headers, "Connection: close");
        curl_easy_setopt(sis_handle, CURLOPT_HTTPHEADER, headers);

        string url = "https://sis.cuhk.edu.cn/psp/csprd/?cmd=login";

        curl_easy_setopt(sis_handle, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(sis_handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(sis_handle, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(sis_handle);

        //向sts.cuhk.edu.cn发送登录请求(POST)
        url = string("https://sts.cuhk.edu.cn/adfs/oauth2/authorize?")
            + "response_type=" + "code" 
            + "&client_id=" + "3f09a73c-33cf-49b8-8f0c-b79ea2f3e83b" 
            + "&redirect_uri=" + "https://sis.cuhk.edu.cn/sso/dologin.html"
            + "&client-request-id=" + "e4ad901b-ac83-4ace-8413-0040020000e8";

        string strdata ="UserName=cuhksz\\" + username + "&Kmsi=true&AuthMethod=FormsAuthentication&Password=" + password ; //POST data
        const char* data = strdata.c_str();
        curl_easy_setopt(sis_handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(sis_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(sis_handle, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(sis_handle);

        char* final_url;
        curl_easy_getinfo(sis_handle, CURLINFO_EFFECTIVE_URL, &final_url);
        //cout << "final url: \n" << final_url << endl; 
        string code = string(final_url).substr(string(final_url).find("code=")+5);
        //cout << "code= \n" << code << endl; 
        
        url = string("https://sis.cuhk.edu.cn/psp/csprd/?")
            + "cmd=" + "login" 
            + "&languageCd=" + "ENG"
            + "&code=" + code;

        //cout << "URL: " << url << endl;
        //生成随机字符串
        // 创建一个随机数生成器，使用随机设备作为种子
        std::random_device rd;
        std::mt19937 gen(rd()); // Mersenne Twister 随机数生成器
    
        // 定义一个均匀分布，范围为 0 到 25（因为大写字母的索引范围是 0 到 25）
        std::uniform_int_distribution<> distrib(0, 25);
    
        // 生成一个由 10 个大写字母组成的字符串
        std::string randomstring;
        for (int i = 0; i < 10; ++i) {
            // 使用生成器随机选择一个字母的索引
            char letter = alphabet[distrib(gen)];
            randomstring += letter;  // 将字母添加到结果字符串中
        }

        //cout << "Randomstring:" << randomstring << endl;

        strdata = 
                string("timezoneOffset=") + "-480" +
                "&ptmode=" +  "f" +
                "&ptlangcd=" + "ENG" +
                "&ptinstalledlang=" + "ENG,ZHT,ZHS" +
                "&userid=" + "CUSZ_SSO_LOGIN" +
                "pwd=" + randomstring +
                "&ptlangsel=" + "ENG";
            
        curl_easy_setopt(sis_handle, CURLOPT_POSTFIELDS, strdata.c_str());
        curl_easy_setopt(sis_handle, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(sis_handle, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(sis_handle);

        // 手动添加 Cookie
        curl_easy_setopt(sis_handle, CURLOPT_COOKIE, "PS_DEVICEFEATURES=width:1728 height:1152 pixelratio:1.25 touch:0 geolocation:1 websockets:1 webworkers:1 datepicker:1 dtpicker:1 timepicker:1 dnd:1 sessionstorage:1 localstorage:1 history:1 canvas:1 svg:1 postmessage:1 hc:0");
        url = "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/s/WEBLIB_PTBR.ISCRIPT1.FieldFormula.IScript_StartPage?&";
        curl_easy_setopt(sis_handle, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(sis_handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(sis_handle, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(sis_handle);


        curl_easy_getinfo(sis_handle, CURLINFO_EFFECTIVE_URL, &final_url);
        cout << "final url: \n" << final_url << endl; 


        if(res != CURLE_OK){
            std::cerr << "Login failed because:" << curl_easy_strerror(res) << endl;
        }
        else if(string(final_url).find("sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/h/?tab=STUDENT_HOMEPAGE")!=string::npos ){
            is_login = true;
            cout << "Login successfully!" << endl;
            return true;
        }
        else{
            std::cerr << "Username or Password incorrect!" << endl;
            return false;
        }

        curl_slist_free_all(headers); // 释放 headers

    }

    std::cerr << "Failed to initialize sis_handle." << endl;
    return false;

}

string SisSystem::getRequest(const string& url)const{
    string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(sis_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(sis_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(sis_handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(sis_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(sis_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        CURLcode res;
        res = curl_easy_perform(sis_handle);  
        if(res != CURLE_OK){
            std::cerr<< "Get request failed:" << curl_easy_strerror(res) << endl;
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
    std::cerr<< "GetRequest failed:Login before get request" << endl;
    return response;
}

string SisSystem::postRequest(const string& url, const string& strdata)const{
    string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(sis_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(sis_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(sis_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        const char* data = strdata.c_str();
        curl_easy_setopt(sis_handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(sis_handle, CURLOPT_FOLLOWLOCATION, 1L);
        CURLcode res;
        res = curl_easy_perform(sis_handle);  
        if(res != CURLE_OK){
            std::cerr<< "Post request failed:" << curl_easy_strerror(res) << endl;
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
    std::cerr<< "PostRequest failed:Login before post request" << endl;
    return response;
}

bool SisSystem::change_info(const string& username, const string& password){
    if(is_login){
        return false;
    }
    this->username = username;
    this->password = password;
    return true;
}


string SisSystem::get_commands()const{

    std::ostringstream result;
    for(auto it = command_list.begin(); it != command_list.end(); ++it ){
        result << *it;
        if(std::next(it) != command_list.end()){
            result << ", ";
        }
    }
    return result.str();
}


string SisSystem::get_course(const string& term)const{
    string rawData = getRequest("https://bb.cuhk.edu.cn/webapps/bb-enhance-BBLEARN/normal/mycourse/search"); 
    vector<string> crouse_name = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[1]/span[2]");
    vector<string> crouse_instructor = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[3]/span[2]");
    vector<string> crouse_term = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[4]/span[2]");

    string total_result = "";
    for(int i = 0 ; i < crouse_name.size(); ++i){
        if(crouse_term[i] == term ){
            total_result+= "Crouse Name: " + crouse_name[i] + "\n";
            total_result+= "Instructor: " + crouse_instructor[i] + "\n";
            total_result+= "Crouse Term: " + crouse_term[i] + "\n\n";
        }
    }

    return total_result;
}

string SisSystem::get_course_id(const string& crouse)const{
    string rawData = getRequest("https://bb.cuhk.edu.cn/webapps/bb-enhance-BBLEARN/normal/mycourse/search"); 
    vector<string> crouse_name = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[1]/span[2]");
    vector<string> crouse_id = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/th/a/@onclick");
    //截取id部分
    std::for_each(crouse_id.begin(), crouse_id.end(), [](string& str) {
        str = str.substr(str.find("id=") + 3, str.find("&url") - str.find("id=")-3);
    }); 
    for(int i = 0 ; i < crouse_id.size(); ++i){
        if(crouse_name[i].find(crouse) != string::npos ){
            return crouse_id[i];
        }
    }
    return "";
}

string SisSystem::get_announcement(const string& crouse, const int number )const{
    string id = get_course_id(crouse);
    if(!id.empty()){
        string data ="method=search&viewChoice=3&editMode=false&tabAction=false&announcementId=&course_id=&context=mybb&internalHandle=my_announcements&searchSelect=" + id; //POST data
        string rawData = postRequest("https://bb.cuhk.edu.cn/webapps/blackboard/execute/announcement", data);
        vector<string> headers = xpathQuery(rawData, "//li[@class='clearfix']/h3");
        vector<string> postTime = xpathQuery(rawData, "//li[@class='clearfix']/div[@class='details']/p[1]");
        vector<string> details = xpathQuery(rawData, "//li[@class='clearfix']/div[@class='details']/div");
        vector<string> posters = xpathQuery(rawData, "//li[@class='clearfix']/div[@class='announcementInfo']/p[1]");
        
        //去除多余空格
        std::for_each(headers.begin(), headers.end(), [](string& str) {
            str = str.substr(str.find_first_not_of(" \t\n\r\v"));
        }); 
        

        string total_result = "";
        for(int i = 0 ; i < headers.size() && i < number; ++i){
            total_result+="Announcement:\n"+headers[i]+"\n"+postTime[i]+"\n"+details[i]+"\n"+posters[i]+"\n";
        }

        return total_result;
    }
    return "";
}

string SisSystem::get_assignment(const string& crouse)const{
    string rawData = getRequest("https://bb.cuhk.edu.cn/webapps/bb-enhance-BBLEARN/normal/mycourse/search"); 
    vector<string> crouse_name = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[1]/span[2]");
    vector<string> crouse_id = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/th/a/@onclick");
    //截取id部分
    std::for_each(crouse_id.begin(), crouse_id.end(), [](string& str) {
        str = str.substr(str.find("id=") + 3, str.find("&url") - str.find("id=")-3);
    }); 

    vector<string> total_assignment_urls;

    for(int i = 0 ; i < crouse_id.size(); ++i){
        if((crouse_name[i].find(crouse) != string::npos)||(crouse=="ALL")){
            string crouseData = getRequest("https://bb.cuhk.edu.cn/webapps/blackboard/execute/launcher?type=Course&id=" + crouse_id[i] + "&url=");
            vector<string> content_urls = xpathQuery(crouseData, "//a[starts-with(@href, '/webapps/blackboard/content/listContent')]/@href");
            for(string contetn_url : content_urls){
                recursive_search_assignments(
                    getRequest("https://bb.cuhk.edu.cn"+contetn_url),
                    total_assignment_urls
                );
            }
        }
    }

    std::stringstream result;
    result << crouse << " assignments:" << endl;
    for(string assignment_url : total_assignment_urls){
        result << parse_assignment_url("https://bb.cuhk.edu.cn"+assignment_url) << endl;
    }

    return result.str();
}


void SisSystem::recursive_search_assignments(const string& data, vector<string>& total_assignmnet_urls)const{

    vector<string> assignment_urls = xpathQuery(data, "//a[starts-with(@href, '/webapps/assignment/uploadAssignment') or starts-with(@href, '/webapps/blackboard/content/launchAssessment') ]/@href");
    vector<string> content_urls = xpathQuery(data, "/html/body/div/div/div/div/div/div/div/ul/li/div/h3/a[starts-with(@href, '/webapps/blackboard/content/listContent')]/@href");

    for(string assignmnet_url : assignment_urls){
        total_assignmnet_urls.push_back(assignmnet_url);
    }

    if(!content_urls.empty()){
        for(string content_url : content_urls){
        
            //cout << "https://bb.cuhk.edu.cn" + content_url << endl;
            recursive_search_assignments(
                getRequest("https://bb.cuhk.edu.cn" + content_url),
                total_assignmnet_urls
            );
        }
    }
}

string SisSystem::vector_toString(const vector<string>& vector)const{
    string result;
    for(string element : vector){
        result+=element+"\n";
    }
    return result;
}

string SisSystem::space_cutter(const string& str)const{
    std::regex pattern("^\\s*(.*?)\\s*$");
    std::smatch match;
    if (std::regex_match(str, match, pattern)) {
        return match[1];
    } else {
        return "";
    }
}

string SisSystem::parse_assignment_url(const string& url)const{
    std::stringstream result;
    string rawData = getRequest(url);
    if(url.find("launchAssessment")!=string::npos){
        string header = space_cutter(xpathQuery(rawData, "//span[@id='pageTitleText']")[0]);
        string title = header.substr(header.find(":")+1);
        string dueinfo = space_cutter(xpathQuery(rawData, "//*[@id='stepcontent1']/ol/li[position()=last()]/div[2]/text()")[0]);

        result << title << endl;

        //提取时间
        int startpos = dueinfo.find("due on ")+7;
        string duedate = dueinfo.substr(startpos);
        int endpos = duedate.find("Test");
        duedate = duedate.substr(0, endpos);
        result << "Duedate:" << duedate <<endl;
        result << "you need visit bb.cuhk.edu.cn to check wether this assignment is done." << endl;

    }
    else{
        vector<string> header = xpathQuery(rawData, "//li[@class='placeholder']/span");
        string status = header[0].substr(0,header[0].find(":"));
        string title = header[0].substr(header[0].find(":")+2);
        title = space_cutter(title);
        result << title << endl;
        if(status.find("Review")!=string::npos){
            result << "Status: Already Upload!" << endl;
            vector<string> grades = xpathQuery(rawData, "//*[@id='aggregateGrade']/@value");
            vector<string> maximum_grades = xpathQuery(rawData, "//*[@id='aggregateGrade_pointsPossible']");
            result << grades[0] << maximum_grades[0] << endl;
            if(grades[0].find("-")!=string::npos){
                result << "Grades not yet updated" << endl;
            }
        }
        if(status.find("Upload")!=string::npos){
            string due = space_cutter(xpathQuery(rawData, "//*[@id='metadata']/div/div/div[1]/div[2]/text()")[0])
                            + space_cutter(xpathQuery(rawData, "//*[@id='metadata']/div/div/div[1]/div[2]/span")[0]);
            string possible_points = space_cutter(xpathQuery(rawData, "//*[@id='metadata']/div/div/div[2]/div[2]/text()")[0]);
            result << "Points Possible: " << possible_points << endl;
            result << "DueDate: " << due << endl;
            if (missDue(due, "%A, %B %d, %Y %I:%M %p" )) {
                result << "Missed the due date!" << endl;
            }
            else{
                result << "Need Upload!" << endl;
            }
        }
    }

    return result.str();

}


bool SisSystem::missDue(const string& content, const string& pattern)const{

    // 定义时间结构体
    std::tm tm = {};
    std::istringstream ss(content);

    // 解析时间字符串（需根据实际格式调整格式符）
    ss >> std::get_time(&tm, pattern.c_str());
    if (ss.fail()) {
        std::cerr << "Failed to parse time string!" << endl;
    }

    // 将 tm 转换为 time_t
    std::time_t input_time_t = std::mktime(&tm);
    if (input_time_t == -1) {
        std::cerr << "Failed to convert to time_t!" << endl;
    }
    auto now_time = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now_time);
    if (input_time_t <= now_time_t) {
        return true;
    }
    
    return false;
    
}

string SisSystem::get_grades(const string& crouse)const{
    string id = get_course_id(crouse);
    if(!id.empty()){
        string url ="https://bb.cuhk.edu.cn/webapps/bb-mygrades-BBLEARN/myGrades?course_id=" + id + "&stream_name=mygrades";
        string rawData = getRequest(url);
        vector<string> name = xpathQuery(rawData, "//div[@id='grades_wrapper']/div/div[@class='cell gradable']/span | //div[@id='grades_wrapper']/div/div[@class='cell gradable']/a ");
        vector<string> grades = xpathQuery(rawData, "//div[@id='grades_wrapper']/div/div[@class='cell grade']/span[1]");
        vector<string> grade_time = xpathQuery(rawData, "//div[@id='grades_wrapper']/div/div[@class='cell activity timestamp']/span[1]");
        vector<string> grade_stamp = xpathQuery(rawData, "//div[@id='grades_wrapper']/div/div[@class='cell activity timestamp']/span[2]");
        

        std::stringstream total_result;
        total_result << crouse << " grades: " << endl;
        for(int i = 0 ; i < name.size(); ++i){
            total_result << space_cutter(name[i]) + " " + space_cutter(grade_stamp[i]) << endl;
            total_result << "Grades: " << space_cutter(grades[i]) << endl;
            total_result << space_cutter(grade_time[i]) << endl;
        }

        return total_result.str();
    }
    return "";
}