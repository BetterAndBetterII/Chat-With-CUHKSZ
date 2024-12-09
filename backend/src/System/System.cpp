#include "../../include/System/System.h"
#include <iostream> //std::cerr
#include <cstring> //memcpy
#include <filesystem> //destructor

using std::string;
using std::vector;
using std::cout;
using std::endl;

System::System(const string& username, const string& password) : curl_global_manager(){
    //初始化变量
    this->username = username;
    this->password = password;
    this->handle = curl_easy_init();
    // 自定义 HTTP 请求头
    this->headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36");
    headers = curl_slist_append(headers, "Connection: close");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    this->is_login = false;

}

System::~System(){
    if(handle){
        curl_easy_cleanup(handle);
        //delete XXXcookie.txt
        std::filesystem::path filepath = cookiefile;  // xxxCookies.txt路径
        try {
            if (std::filesystem::remove(filepath)) {
                //cout << "CookieFile deleted successfully\n";
            } else {
                //cout << "CookieFile does not exist or could not be deleted\n";
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error: " << e.what() << "\n";
        } 
    }
}

//callback functions implementation
size_t System::ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    return size * nmemb;  // 只是返回数据大小，忽略响应体
}

size_t System::write_callback(char *data, size_t size, size_t nmemb, void *clientp)
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

size_t System::header_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    // 计算头部的大小
    size_t header_size = size * nmemb;
    // 打印响应头
    cout << "---Response Header----" << endl;
    printf("%.*s", (int)header_size, ptr);
    return header_size; // 返回处理的字节数
}

size_t System::debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr) {
    (void)handle;  // 如果不需要 handle，可以忽略
    (void)userptr; // 如果不需要 userptr，可以忽略

    switch (type) {
        case CURLINFO_TEXT:
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
            break;
        default:
            break;
    }
    return 0;
}

vector<string> System::xpathQuery(const string& xmlContent, const string& xpathExpr)const {
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

string System::getRequest(const string& url)const{
    string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&chunk);
        CURLcode res;
        res = curl_easy_perform(handle);  
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

string System::postRequest(const string& url, const string& strdata)const{
    string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&chunk);
        const char* data = strdata.c_str();
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        CURLcode res;
        res = curl_easy_perform(handle);  
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

bool System::change_info(const string& username, const string& password){
    if(is_login){
        return false;
    }
    this->username = username;
    this->password = password;
    return true;
}

string System::get_commands()const{

    std::ostringstream result;
    for(auto it = command_list.begin(); it != command_list.end(); ++it ){
        result << *it;
        if(std::next(it) != command_list.end()){
            result << ", ";
        }
    }
    return result.str();
}