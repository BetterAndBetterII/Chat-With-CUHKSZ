#include "../../include/System/Libary.h"
#include <iostream> //std::cerr
#include <fstream>
#include <sstream> 
#include <map>
#include <cstring>
#include <regex> //space_cutter()
#include <filesystem> //destructor

using std::string;
using std::vector;
using std::cout;
using std::endl;

LibarySystem::LibarySystem() : curl_global_manager(){
    //初始化变量
    this->command_list = {
        "search"
    };
    this->libary_handle = curl_easy_init();
}

LibarySystem::~LibarySystem(){
    if(libary_handle){
        curl_easy_cleanup(libary_handle);
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

string LibarySystem::getRequest(const string& url)const{
    string response = "";
    Memory chunk = {nullptr, 0};
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

string LibarySystem::postRequest(const string& url, const string& strdata)const{
    string response = "";
    Memory chunk = {nullptr, 0};
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

string LibarySystem::search(const std::string& keyword, const int& limit, const string& tab)const{

    std::map<string, string> tab_map={
        {"Everything","Everything"},
        {"PrintBooks/Journals","LibraryCatalog"},
        {"Articles/eBooks","CentralIndex"}
    };
    std::map<string, string> scope_map={
        {"Everything","MyInst_and_CI"},
        {"PrintBooks/Journals","MyInstitution"},
        {"Articles/eBooks","CentralIndex"}
    };
    string mapped_tab = tab_map[tab];
    string scope = scope_map[tab];

    //处理输入的空格
    char* escaped_keyword = curl_easy_escape(libary_handle, keyword.c_str(), static_cast<int>(keyword.length()));

    string param = 
        string("acTriggered=") + "false" +          //+只在string定义
        "&blendFacetsSeparately=" + "false" +
        "&citationTrailFilterByAvailability=" + "true" +
        "&disableCache=" + "false" +
        "&getMore=" + "0" +
        "&inst=" + "86CUHKSZ_INST" +
        "&isCDSearch=" + "false" +
        "&lang=" + "en" +
        "&limit=" + std::to_string(limit) +
        "&newspapersActive=" + "false" +
        "&newspapersSearch=" + "false" +
        "&offset=" + "0" +
        "&otbRanking=" + "false" +
        "&pcAvailability=" + "false" +
        "&q=" + "any,contains," + escaped_keyword +
        "&qExclude=" + "" +
        "&qInclude=" + "" +
        "&rapido=" + "false" +
        "&refEntryActive=" + "false" +
        "&rtaLinks=" + "true" +
        "&scope=" + scope +
        "&searchInFulltextUserSelection=" + "true" +
        "&skipDelivery=" + "Y" +
        "&sort=" + "rank" +
        "&tab=" + mapped_tab +
        "&vid=" + "86CUHKSZ_INST:86CUHKSZ";
        
    string url = "https://cuhksz.primo.exlibrisgroup.com.cn/primaws/rest/pub/pnxs?"+ param;

    //处理unsafe legacy renegotiation problem
    curl_easy_setopt(libary_handle, CURLOPT_SSL_CTX_FUNCTION, ssl_ctx_callback);
    string response = getRequest(url);

    curl_free(escaped_keyword);

    vector<Info> results;
    nlohmann::json json_res = nlohmann::json::parse(response);


    for(const auto& doc : json_res["docs"]){
        Info info;
        if (doc["pnx"]["display"].contains("creator"))
                info.creator = doc["pnx"]["display"]["creator"][0];
        if (doc["pnx"]["display"].contains("subject"))
                info.subject = doc["pnx"]["display"]["subject"][0];
        if (doc["pnx"]["display"].contains("title"))
                info.title = doc["pnx"]["display"]["title"][0];
        else cout << "Json didn't find title" << endl;
        if (doc["pnx"]["display"].contains("publisher"))
                info.publisher = doc["pnx"]["display"]["publisher"][0];
        if (doc["pnx"]["display"].contains("type"))
                info.type = doc["pnx"]["display"]["type"][0];
        results.push_back(info);
    }

    string str_result="====Search Result====\n";
    for(Info info : results){
        if (info.title.has_value()) {
            str_result+= "Title: " + info.title.value() + "\n";
        }
        if (info.publisher.has_value()) {
            str_result+= "Publisher: " + info.publisher.value() + "\n";
        }
        if (info.type.has_value()) {
            str_result+= "Type: " + info.type.value() + "\n";
        }
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