//
// Created by Gary on 24-10-23.
// Complemented by Wen on 24-11-18.
//

#include "../../include/System/Blackboard.h"
#include <iostream> //std::cerr
#include <regex> //space_cutter()
#include <chrono> //missDue() 
#include <filesystem> //missDue()->get_time()

using std::string;
using std::vector;
using std::cout;
using std::endl;

BlackBoardSystem::BlackBoardSystem(const string& username, const string& password) : System(username, password){
    //初始化变量
    this->command_list = {
        "get_course",
        "get_annoucement",
        "get_assignment",
        "get_grades",
        "get_name"
    };

}

BlackBoardSystem::~BlackBoardSystem(){}


//login function implementation
bool BlackBoardSystem::login(){
    if(is_login){
        return true;
    }

    //尝试登录
    if(handle){

        //忽略登录过程返回的响应体（注释下行可把响应体打印到终端）
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, ignore_calback);

        // 启用自动cookie处理，指定cookie文件
        cookiefile = username + "bbCookies.txt";
        curl_easy_setopt(handle, CURLOPT_COOKIEJAR,  cookiefile.c_str());  // 保存cookies
        curl_easy_setopt(handle, CURLOPT_COOKIEFILE, cookiefile.c_str()); // 发送保存的cookies

        //向sts.cuhk.edu.cn发送登录请求(POST)
        string strdata ="UserName=cuhksz\\" + username + "&Kmsi=true&AuthMethod=FormsAuthentication&Password=" + password ; //POST data
        const char* data = strdata.c_str();
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_URL, "https://sts.cuhk.edu.cn/adfs/oauth2/authorize?response_type=code&client_id=4b71b947-7b0d-4611-b47e-0ec37aabfd5e&redirect_uri=https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode&client-request-id=dd5ffbba-e761-453e-e58f-014001000089");
        CURLcode res;
        res = curl_easy_perform(handle);
        
        //通过重定向次数判断是否成功登录
        long redirect_count;
        curl_easy_getinfo(handle, CURLINFO_REDIRECT_COUNT, &redirect_count);
        std::cout<<username<<"   "<<password<<std::endl;
        if(res != CURLE_OK){
            std::cerr << "Login failed because:" << curl_easy_strerror(res) << endl;
        }
        else if(redirect_count > 0 ){
            is_login = true;
            cout << "Login successfully!" << endl;
            return true;
        }
        else{
            std::cerr << "Username or Password incorrect!" << endl;
            return false;
        }

    }

    std::cerr << "Failed to initialize handle." << endl;
    return false;

}




std::string BlackBoardSystem::get_course(const std::string& term){
    if (!login()) {
        return "Invalid username or password!";
    }
    std::string rawData = getRequest("https://bb.cuhk.edu.cn/webapps/bb-enhance-BBLEARN/normal/mycourse/search"); 
    std::vector<std::string> crouse_name = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[1]/span[2]");
    std::vector<std::string> crouse_instructor = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[3]/span[2]");
    std::vector<std::string> crouse_term = xpathQuery(rawData, "//*[@id[starts-with(., 'listContainer_row:')]]/td[4]/span[2]");

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

string BlackBoardSystem::get_course_id(const string& crouse)const{
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

string BlackBoardSystem::get_announcement(const string& crouse, const int number ){
    if (!login()) {
        return "Invalid username or password!";
    }
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

string BlackBoardSystem::get_assignment(const string& crouse){
    if (!login()) {
        return "Invalid username or password!";
    }
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


void BlackBoardSystem::recursive_search_assignments(const string& data, vector<string>& total_assignmnet_urls)const{

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

string BlackBoardSystem::space_cutter(const string& str)const{
    std::regex pattern("^\\s*(.*?)\\s*$");
    std::smatch match;
    if (std::regex_match(str, match, pattern)) {
        return match[1];
    } else {
        return "";
    }
}

string BlackBoardSystem::parse_assignment_url(const string& url)const{
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


bool BlackBoardSystem::missDue(const string& content, const string& pattern)const{

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

string BlackBoardSystem::get_grades(const string& crouse){
    if (!login()) {
        return "Invalid username or password!";
    }
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

string BlackBoardSystem::get_name(){
    if (!login()) {
        return "Invalid username or password!";
    }

    string url ="https://bb.cuhk.edu.cn/webapps/portal/execute/tabs/tabAction?tab_tab_group_id=_1_1";
    string rawData = getRequest(url);

    string name = xpathQuery(rawData, "//*[@id='global-nav-link']/text()")[0];

    return name;
}