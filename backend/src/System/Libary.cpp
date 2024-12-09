#include "../../include/System/Libary.h"
#include <iostream> //std::cerr cout

using std::string;
using std::vector;
using std::cout;
using std::endl;

LibarySystem::LibarySystem(const string& username, const string& password) : System(username, password){ //libary 未实现login
    //初始化变量
    this->command_list = {
        "search"
    };
}

LibarySystem::~LibarySystem(){}

CURLcode LibarySystem::ssl_ctx_callback(CURL *curl, void *ssl_ctx, void *userptr)
{
  long bitmask = SSL_CTX_set_options((SSL_CTX *)ssl_ctx,
    SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION);

  return (bitmask & SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION) ?
    CURLE_OK : CURLE_ABORTED_BY_CALLBACK;
}

bool LibarySystem::login(){
    is_login=true;
    return is_login;
}

string LibarySystem::search(const std::string& keyword, const int& limit, const string& tab){
    if (!login()) {
        return "Invalid username or password!";
    }

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
    char* escaped_keyword = curl_easy_escape(handle, keyword.c_str(), static_cast<int>(keyword.length()));

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
    curl_easy_setopt(handle, CURLOPT_SSL_CTX_FUNCTION, ssl_ctx_callback);
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