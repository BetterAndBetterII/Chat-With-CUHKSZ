//
// Created by Wen on 24-11-27.
//
#include "../../include/System/Sis.h"
#include <iostream> //std::cerr
#include <sstream> 
#include <fstream>
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
        "get_schedule",
    };
    this->username = username;
    this->password = password;
    this->sis_handle = curl_easy_init();
    // 自定义 HTTP 请求头
    this->headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36");
    headers = curl_slist_append(headers, "Connection: close");
    curl_easy_setopt(sis_handle, CURLOPT_HTTPHEADER, headers);
    this->is_login = false;

}

SisSystem::~SisSystem(){
    if(sis_handle){
        curl_easy_cleanup(sis_handle);
        curl_slist_free_all(headers); // 释放 headers
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
        cout << "No results\n";
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
        //打印详细输出
        //curl_easy_setopt(sis_handle, CURLOPT_VERBOSE, 1L);
        //curl_easy_setopt(sis_handle, CURLOPT_DEBUGFUNCTION, debug_callback);

        // 启用自动cookie处理，指定cookie文件
        cookiefile = username + "sisCookies.txt";
        curl_easy_setopt(sis_handle, CURLOPT_COOKIEJAR,  cookiefile.c_str());  // 保存cookies
        curl_easy_setopt(sis_handle, CURLOPT_COOKIEFILE, cookiefile.c_str()); // 发送保存的cookies

        //向sts.cuhk.edu.cn发送登录请求(POST)
        string url = string("https://sts.cuhk.edu.cn/adfs/oauth2/authorize?")
            + "response_type=" + "code" 
            + "&client_id=" + "3f09a73c-33cf-49b8-8f0c-b79ea2f3e83b" 
            + "&redirect_uri=" + "https://sis.cuhk.edu.cn/sso/dologin.html"
            + "&client-request-id=" + "e4ad901b-ac83-4ace-8413-0040020000e8";

        string strdata ="UserName=cuhksz\\" + username + "&Kmsi=true&AuthMethod=FormsAuthentication&Password=" + password ; //POST data
        curl_easy_setopt(sis_handle, CURLOPT_POSTFIELDS, strdata.c_str());
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

        //生成随机字符串用于第二阶段的post
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
                "&pwd=" + randomstring +
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
        //cout << "final url: \n" << final_url << endl; 


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


string SisSystem::get_schedule()const{

    string url = string("https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_SCHD_W.GBL?") +
    "FolderPath=PORTAL_ROOT_OBJECT.CO_EMPLOYEE_SELF_SERVICE.HCCC_ENROLLMENT.HC_SSR_SSENRL_SCHD_W_GBL"+
    "&IsFolder=false"+
    "&IgnoreParamTempl=FolderPath,IsFolder";

    string rawData = getRequest(url);

    //std::ofstream outFile;
    //outFile.open("output.html");
    //outFile << rawData;
    //outFile.close();

    //截取课表部分（否则xpath查询不到）
    size_t startpos = rawData.find("<table cellspacing='0' cellpadding='2' width='100%' class='PSLEVEL1GRIDNBO' id='WEEKLY_SCHED_HTMLAREA'>");
    if (startpos == std::string::npos) {
        std::cout << "Can not find class table! from page" << std::endl;
    }
    rawData = rawData.substr(startpos);
    size_t endpos = rawData.find("class='PSLEVEL3GRID'>&nbsp;</td></tr></table></div>");
    rawData = rawData.substr(0, endpos+45);
    rawData = "<html><head></head><body>"+rawData+"</body></html>";
    //cout << "rawData:\n" <<rawData <<endl;

    vector<vector<string>> table(16, vector<string>(8));
    table[0][0]="Time";table[0][1]="Monday";table[0][2]="Tuesday";table[0][3]="Wednesday";table[0][4]="Thursday";table[0][5]="Friday";table[0][6]="Saturday";table[0][7]="Sunday";
    
    for(int row = 2 ; row <= 16 ; row++){
        //cout << "===row=" << row << "===" << endl;
        int item_counter = 0 ;
        if(row!=2){
            for(string item : table[row-1]){if(item!=""){item_counter++;}}
        }
        //cout << "itemnum: " << item_counter << endl;;
        int table_index = 0;
        for(int col = 1 ; col <= 8-item_counter ; col++){
            std::stringstream xpath;
            xpath <<  "/html/body/table/tr[" << row << "]/td[" << col << "]/";
            //cout << xpath.str() << endl;;
            while(table[row-1][table_index]!=""){table_index++;}
            //课程格
            if(xpathQuery(rawData, xpath.str()+"@class")[0]=="SSSWEEKLYBACKGROUND"){
                table[row-1][table_index]=xpathQuery(rawData, xpath.str()+"/span")[0];;
                if(xpathQuery(rawData, xpath.str()+"@rowspan")[0]=="2"){
                    table[row][table_index]="*";
                }
                if(xpathQuery(rawData, xpath.str()+"@rowspan")[0]=="3"){
                    table[row][table_index]="*";
                    table[row+1][table_index]="*";
                }
            }
            //时间格
            if(xpathQuery(rawData, xpath.str()+"@class")[0]=="SSSWEEKLYTIMEBACKGROUND"){
                table[row-1][table_index]=xpathQuery(rawData, xpath.str()+"/span")[0];
            }

            table_index+=1;            
            
        }

    }

    //返回string
    string final_result;
    for(int day=1 ; day<=7 ; day++){
        final_result+= table[0][day] + ":\n";
        for(int time_index = 1 ; time_index<=15 ; time_index++){
            if(table[time_index][day]!="*"&&table[time_index][day]!=""){
                //cout << "table[time_index][day]= " << table[time_index][day] << endl;;
                final_result+=course_parser(table[time_index][day]);
            }
        } 
        final_result+="\n";
    }

    return final_result;
}

string SisSystem::vector_toString(const vector<string>& vector)const{
    string result;
    for(string element : vector){
        result+=element+"\n";
    }
    return result;
}

void SisSystem::printTable(const vector<vector<string>>& Vector)const{
    cout<< "[Table]" <<endl;
    string result;
    for(vector<string> rowvector : Vector){
        string rowstr="";
        for(string item : rowvector){
            if(item==""){
                //注意空值填入'x'
                rowstr+="x";
            }
            else{
                rowstr+=item;
            }
        }
        cout << rowstr << endl;

    }
}

string SisSystem::course_parser(const string& courseinfo)const{
    string result;
    std::regex pattern("^([A-Z]{3}\\s[0-9]{4}\\s-\\s[LT]\\d+)(Lecture|Tutorial)(\\d{1,2}:\\d{2}(AM|PM) - \\d{1,2}:\\d{2}(AM|PM))(.+)");
    std::smatch match;
    if (std::regex_search(courseinfo, match, pattern)) {
        //std::cout << "Matched" << std::endl;
        if (match.size() == 7) { // 有 6 个捕获组，match[0] 是整个匹配内容
            string course_name = match[1];
            string type = match[2];
            string time = match[3];
            string location = match[6];

            result += "Course: " + course_name + "\n" 
                + "Type: " + type + "\n"
                + "Duration: " + time + "\n"
                + "Location: " + location + "\n";
        }
    } else {
        std::cout << "No match found." << std::endl;
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
