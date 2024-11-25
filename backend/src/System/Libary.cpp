#include "../../include/System/Libary.h"
#include <iostream> //std::cerr
#include <fstream>
#include <sstream> 
#include <cstring>
#include <regex> //space_cutter()
#include <filesystem> //destructor

using std::string;
using std::vector;
using std::cout;
using std::endl;

LibarySystem::LibarySystem(const string& username, const string& password) : curl_global_manager(){
    //初始化变量
    this->command_list = {
        "get_course",
        "get_annoucement",
        "get_assignment",
        "get_grades"
    };
    this->username = username;
    this->password = password;
    this->libary_handle = curl_easy_init();
    this->is_login = false;

}

LibarySystem::~LibarySystem(){
    if(libary_handle){
        curl_easy_cleanup(libary_handle);
        //delete bbcookie.txt
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
        //调试：检测libary_handle是否清除
        //cout << "libary_handle cleaned up" << endl;
    }
}

//callback functions implementation
size_t LibarySystem::ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    return size * nmemb;  // 只是返回数据大小，忽略响应体
}

size_t LibarySystem::write_callback(char *data, size_t size, size_t nmemb, void *clientp)
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

CURLcode LibarySystem::ssl_ctx_callback(CURL *curl, void *ssl_ctx, void *userptr)
{
  long bitmask = SSL_CTX_set_options((SSL_CTX *)ssl_ctx,
    SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION);

  return (bitmask & SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION) ?
    CURLE_OK : CURLE_ABORTED_BY_CALLBACK;
}

vector<string> LibarySystem::xpathQuery(const string& xmlContent, const string& xpathExpr)const {
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
bool LibarySystem::login(){
    if(is_login){
        return true;
    }

    //尝试登录
    if(libary_handle){

        //忽略登录过程返回的响应体（注释下行可把响应体打印到终端）
        curl_easy_setopt(libary_handle, CURLOPT_WRITEFUNCTION, ignore_calback);

        // 启用自动cookie处理，指定cookie文件
        cookiefile = username + "libraryCookies.txt";
        curl_easy_setopt(libary_handle, CURLOPT_COOKIEJAR,  cookiefile.c_str());  // 保存cookies
        curl_easy_setopt(libary_handle, CURLOPT_COOKIEFILE, cookiefile.c_str()); // 发送保存的cookies

        /*//向libary发送登录请求(POST)
        string strdata ="UserName=cuhksz\\" + username + "&Kmsi=true&AuthMethod=FormsAuthentication&Password=" + password ; //POST data
        const char* data = strdata.c_str();
        curl_easy_setopt(libary_handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(libary_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(libary_handle, CURLOPT_URL, "https://sts.cuhk.edu.cn/adfs/oauth2/authorize?response_type=code&client_id=4b71b947-7b0d-4611-b47e-0ec37aabfd5e&redirect_uri=https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode&client-request-id=dd5ffbba-e761-453e-e58f-014001000089");
        CURLcode res;
        res = curl_easy_perform(libary_handle);
        
        //通过重定向次数判断是否成功登录
        long redirect_count;
        curl_easy_getinfo(libary_handle, CURLINFO_REDIRECT_COUNT, &redirect_count);
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
        } */
       is_login = true;
       return true;

    }

    std::cerr << "Failed to initialize libary_handle." << endl;
    return false;

}

string LibarySystem::getRequest(const string& url)const{
    string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(libary_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(libary_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(libary_handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(libary_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(libary_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        CURLcode res;
        res = curl_easy_perform(libary_handle);  
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

string LibarySystem::postRequest(const string& url, const string& strdata)const{
    string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(libary_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(libary_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(libary_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        const char* data = strdata.c_str();
        curl_easy_setopt(libary_handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(libary_handle, CURLOPT_FOLLOWLOCATION, 1L);
        CURLcode res;
        res = curl_easy_perform(libary_handle);  
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

bool LibarySystem::change_info(const string& username, const string& password){
    if(is_login){
        return false;
    }
    this->username = username;
    this->password = password;
    return true;
}


string LibarySystem::get_commands()const{

    std::ostringstream result;
    for(auto it = command_list.begin(); it != command_list.end(); ++it ){
        result << *it;
        if(std::next(it) != command_list.end()){
            result << ", ";
        }
    }
    return result.str();
}

string LibarySystem::vector_toString(const vector<string>& vector)const{
    string result;
    for(string element : vector){
        result+=element+"\n";
    }
    return result;
}

string LibarySystem::space_cutter(const string& str)const{
    std::regex pattern("^\\s*(.*?)\\s*$");
    std::smatch match;
    if (std::regex_match(str, match, pattern)) {
        return match[1];
    } else {
        return "";
    }
}

string LibarySystem::search(const std::string& keyword)const{

    char* escaped_keyword = curl_easy_escape(libary_handle, keyword.c_str(), static_cast<int>(keyword.length()));

    string param = "acTriggered=false&blendFacetsSeparately=false&citationTrailFilterByAvailability=true&disableCache=false&getMore=0&inst=86CUHKSZ_INST&isCDSearch=false&lang=en&limit=10&newspapersActive=false&newspapersSearch=false&offset=0&otbRanking=false&pcAvailability=false&q=any,contains,"
        +string(escaped_keyword)
        +"&qExclude=&qInclude=&rapido=false&refEntryActive=false&rtaLinks=true&scope=MyInst_and_CI&searchInFulltextUserSelection=false&skipDelivery=Y&sort=rank&tab=Everything&vid=86CUHKSZ_INST:86CUHKSZ"; 
        
    string url = "https://cuhksz.primo.exlibrisgroup.com.cn/primaws/rest/pub/pnxs?"+ param;

    //处理unsafe legacy renegotiation problem
    curl_easy_setopt(libary_handle, CURLOPT_SSL_CTX_FUNCTION, ssl_ctx_callback);
    string response = getRequest(url);

    curl_free(escaped_keyword);

    vector<Info> results;
    nlohmann::json json_res = nlohmann::json::parse(response);

    for(const auto& doc : json_res["docs"]){
        Info info = {doc["pnx"]["display"]["title"][0],doc["pnx"]["display"]["publisher"][0],doc["pnx"]["display"]["type"][0]};
        if (doc["pnx"]["display"].contains("creator"))
                info.creator = doc["pnx"]["display"]["creator"][0];
        if (doc["pnx"]["display"].contains("subject"))
                info.subject = doc["pnx"]["display"]["subject"][0];
        results.push_back(info);
    }

    string str_result="====Search Result====\n";
    for(Info info : results){
        str_result+=info.title + '\n'+ 
            "Publisher: " +info.publisher + "\n" +
            "Type: " + info.type + "\n";
        if (info.creator.has_value()) {
            str_result+= "Creator: " + info.creator.value() + "\n";
        }
        if (info.subject.has_value()) {
            str_result+= "Subject: " + info.subject.value() + "\n";
        }
        str_result+="\n";
    }
    return str_result; 
}