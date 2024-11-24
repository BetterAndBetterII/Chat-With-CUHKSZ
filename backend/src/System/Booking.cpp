//
// Created by Gary on 24-11-23.
//

#include "../../include/System/Booking.h"

//fieldEvent的构造函数实现
FieldEvent::FieldEvent(
    std::string  field_id,
    std::string  field_name,
    const std::vector<BookEvent>& book_info, 
    const std::map<std::string, std::string>& places
    )
    : field_id(std::move(field_id)), field_name(std::move(field_name)), book_info(book_info), places(places){}

//is_available的实现
std::vector<std::string> FieldEvent::is_available(const std::string& start_time, const std::string& end_time)const{
    // 将输入的时间字符串转换为时间戳
    // auto start_time_chrono = std::chrono::system_clock::from_time_t(std::stoll(start_time));
    // auto end_time_chrono = std::chrono::system_clock::from_time_t(std::stoll(end_time));

    // 按placeID对预订信息进行分组
    std::map<std::string, std::vector<BookEvent>> book_info_dict;
    
    // 初始化每个场地的预订信息列表
    for(const auto& place_pair : places) {
        book_info_dict[place_pair.first] = std::vector<BookEvent>();
    }

    // 将预订信息按场地ID分组
    for(const auto& book : book_info) {
        book_info_dict[book.placeID].push_back(book);
    }

    // 创建可用场地列表
    std::vector<std::string> available_place;
    for(const auto& place_pair : places) {
        available_place.push_back(place_pair.first);
    }

    // 检查每个场地的可用性
    for(const auto& dict_pair : book_info_dict) {
        const std::string& placeID = dict_pair.first;
        const std::vector<BookEvent>& book_info = dict_pair.second;
        
        if(!is_available(start_time, end_time, book_info)) {
            // 从可用场地列表中移除不可用的场地
            available_place.erase(
                std::remove(available_place.begin(), available_place.end(), placeID),
                available_place.end()
            );
        }
    }
    return available_place;
}

//is_available的私有实现
bool FieldEvent::is_available(const std::string& start_time, const std::string& end_time, const std::vector<BookEvent>& book_info)const{
    for (const auto& book : book_info){
        if ((book.startTime < start_time && start_time < book.endTime) or (book.startTime < end_time && end_time < book.endTime)
                or (start_time < book.startTime && book.startTime < end_time) or (start_time < book.endTime && book.endTime < end_time)){
            return false;
        }
    }
    return true;
}

BookingSystem::BookingSystem(const std::string& username, const std::string& password) : curl_global_manager(){
    //初始化变量
    this->command_list = {
        "retrieve_field_info"
    };
    this->username = username;
    this->password = password;
    this->booking_handle = curl_easy_init();
    this->is_login = false;
}

BookingSystem::~BookingSystem(){
    if(booking_handle){
        curl_easy_cleanup(booking_handle);
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
        //调试：检测booking_handle是否清除
        //std::cout << "booking_handle cleaned up" << std::endl;
    }
}

//callback functions implementation
size_t BookingSystem::ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    return size * nmemb;  // 只是返回数据大小，忽略响应体
}

size_t BookingSystem::write_callback(char *data, size_t size, size_t nmemb, void *clientp)
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

std::vector<std::string> BookingSystem::xpathQuery(const std::string& xmlContent, const std::string& xpathExpr)const {
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
bool BookingSystem::login(){
    if(is_login){
        return true;
    }

    //尝试登录
    if(booking_handle){

        //忽略登录过程返回的响应体（注释下行可把响应体打印到终端）
        curl_easy_setopt(booking_handle, CURLOPT_WRITEFUNCTION, ignore_calback);

        // 启用自动cookie处理，指定cookie文件
        cookiefile = username + "bookingCookies.txt";
        curl_easy_setopt(booking_handle, CURLOPT_COOKIEJAR,  cookiefile.c_str());  // 保存cookies
        curl_easy_setopt(booking_handle, CURLOPT_COOKIEFILE, cookiefile.c_str()); // 发送保存的cookies

        //向sts.cuhk.edu.cn发送登录请求(POST)
        std::string strdata ="UserName=cuhksz\\" + username + "&Kmsi=true&AuthMethod=FormsAuthentication&Password=" + password ; //POST data
        const char* data = strdata.c_str();
        curl_easy_setopt(booking_handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(booking_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(booking_handle, CURLOPT_URL, "https://sts.cuhk.edu.cn/adfs/oauth2/authorize?response_type=code&client_id=caf5aded-3f28-4b64-b836-4451312e1ea3&redirect_uri=https://booking.cuhk.edu.cn/sso/code&client-request-id=f8739f9e-124f-4096-8b34-0140020000bb");
        CURLcode res;
        res = curl_easy_perform(booking_handle);
        
        //通过重定向次数判断是否成功登录
        long redirect_count;
        curl_easy_getinfo(booking_handle, CURLINFO_REDIRECT_COUNT, &redirect_count);
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

    std::cerr << "Failed to initialize booking_handle." << std::endl;
    return false;

}

std::string BookingSystem::getRequest(const std::string& url)const{
    std::string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(booking_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(booking_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(booking_handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(booking_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(booking_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        CURLcode res;
        res = curl_easy_perform(booking_handle);  
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

std::string BookingSystem::postRequest(const std::string& url, const std::string& strdata)const{
    std::string response = "";
    Memory chunk = {nullptr, 0};
    if(is_login){
        curl_easy_setopt(booking_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(booking_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(booking_handle, CURLOPT_WRITEDATA, (void*)&chunk);
        const char* data = strdata.c_str();
        curl_easy_setopt(booking_handle, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(booking_handle, CURLOPT_FOLLOWLOCATION, 1L);
        CURLcode res;
        res = curl_easy_perform(booking_handle);  
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

bool BookingSystem::change_info(const std::string& username, const std::string& password){
    if(is_login){
        return false;
    }
    this->username = username;
    this->password = password;
    return true;
}


std::string BookingSystem::get_commands()const{

    std::ostringstream result;
    for(auto it = command_list.begin(); it != command_list.end(); ++it ){
        result << *it;
        if(std::next(it) != command_list.end()){
            result << ", ";
        }
    }
    return result.str();
}

FieldEvent BookingSystem::retrieve_field_info(const std::string& field_name, const std::string& start_time, const std::string& end_time)const{
    if (field_dict.find(field_name) == field_dict.end()){
        std::cerr << "Field name not found in field_dict" << std::endl;
        return {"", "", {}, {}};
    }
    std::string field_id = field_dict.at(field_name);
    std::string url = "https://booking.cuhk.edu.cn/a/field/book/bizFieldBookField/eventsV1?ftId=" + field_id + "&startTime=" + start_time + "&endTime=" + end_time + "&reBookMainId=&jsonStr=[]&fitUseStr=";
    std::string rawData = getRequest(url);
    return FieldEvent(field_id, field_name, _parse_field_info(rawData), get_field_places(field_id));
}

std::vector<BookEvent> BookingSystem::_parse_field_info(const std::string& raw_data){
    std::vector<BookEvent> info_list;
    
    // 解析JSON
    nlohmann::json json_data = nlohmann::json::parse(raw_data);

    // 处理event列表
    if(json_data.contains("event")) {
        for (const auto& event : json_data["event"]) {
            std::string placeID = event["fId"].get<std::string>();
            if (placeID.empty()) continue;

            std::string start_time_str = event["startTime"].get<std::string>();
            std::string end_time_str = event["endTime"].get<std::string>();
            std::string booker = event["userName"].get<std::string>();
            std::string reason = event["theme"].get<std::string>();

            BookEvent book_event{
                placeID,
                start_time_str,
                end_time_str,
                booker,
                reason
            };
            info_list.push_back(book_event);
        }
    }

    // 处理lockEvent列表
    if(json_data.contains("lockEvent")) {
        for (const auto& lock_event : json_data["lockEvent"]) {
            std::string placeID = lock_event["fId"].get<std::string>();
            if (placeID.empty()) continue;

            std::string start_time_str = lock_event["startTime"].get<std::string>();
            std::string end_time_str = lock_event["endTime"].get<std::string>();
            std::string booker = "Locked";
            std::string reason = lock_event["reasons"].get<std::string>();

            BookEvent book_event{
                placeID,
                start_time_str,
                end_time_str,
                booker,
                reason
            };
            info_list.push_back(book_event);
        }
    }

    // 将结果转换为JSON字符串返回
    // nlohmann::json result = nlohmann::json::array();
    // for (const auto& event : info_list) {
    //     nlohmann::json eventObj;
    //     eventObj["placeID"] = event.placeID;
    //     eventObj["startTime"] = event.startTime;
    //     eventObj["endTime"] = event.endTime;
    //     eventObj["booker"] = event.booker;
    //     eventObj["reason"] = event.reason;
    //     result.push_back(eventObj);
    // }

    return info_list;
}

std::map<std::string, std::string> BookingSystem::_parse_field_places(const std::string& raw_data){
    std::map<std::string, std::string> places;
    
    // 将字符串形式的HTML内容解析为libxml2文档对象
    xmlDocPtr doc = htmlReadMemory(raw_data.c_str(), raw_data.size(), NULL, "utf-8", HTML_PARSE_RECOVER|HTML_PARSE_NOERROR);
    if (doc == nullptr) {
        std::cerr << "Failed to parse HTML document\n";
        return places;
    }

    // 创建XPath上下文
    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    if (context == nullptr) {
        std::cerr << "Failed to create XPath context\n";
        xmlFreeDoc(doc);
        return places;
    }

    // 查找所有option元素
    xmlXPathObjectPtr result = xmlXPathEvalExpression((const xmlChar*)"//*[@id='fieldSelect']/option", context);
    if (result == nullptr) {
        std::cerr << "Failed to evaluate XPath expression\n";
        xmlXPathFreeContext(context);
        xmlFreeDoc(doc);
        return places;
    }

    if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        xmlNodeSetPtr nodes = result->nodesetval;
        for (int i = 0; i < nodes->nodeNr; i++) {
            xmlNodePtr node = nodes->nodeTab[i];
            
            // 获取value属性
            xmlChar* place_id = xmlGetProp(node, (const xmlChar*)"value");
            if (place_id == nullptr || xmlStrlen(place_id) == 0) {
                if (place_id) xmlFree(place_id);
                continue;
            }

            // 获取文本内容
            xmlChar* name = xmlNodeGetContent(node);
            if (name) {
                places[reinterpret_cast<char*>(place_id)] = reinterpret_cast<char*>(name);
                xmlFree(name);
            }
            xmlFree(place_id);
        }
    }

    // 释放资源
    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);

    return places;
}

std::map<std::string, std::string> BookingSystem::get_field_places(const std::string field_id)const{
    std::string url = "https://booking.cuhk.edu.cn/a/field/client/main";
    
    // 构建POST数据
    std::string post_data = "id=" + field_id + "&bookType=0&personTag=Student";
    
    // 发送POST请求
    std::string response = postRequest(url, post_data);
    
    // 解析响应数据
    return _parse_field_places(response);
}
