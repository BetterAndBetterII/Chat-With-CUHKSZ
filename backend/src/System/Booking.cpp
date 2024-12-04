//
// Created by Gary on 24-11-23.
//

#include <utility>

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

//is_available使用的辅助函数
bool FieldEvent::is_available(const std::string& start_time, const std::string& end_time, const std::vector<BookEvent>& book_info)const{
    // 将日期时间字符串转换为tm结构
    std::tm start_tm = {};
    std::tm end_tm = {};
    std::istringstream start_ss(start_time);
    std::istringstream end_ss(end_time);
    
    start_ss >> std::get_time(&start_tm, "%Y-%m-%d %H:%M");
    end_ss >> std::get_time(&end_tm, "%Y-%m-%d %H:%M");
    
    // 转换为time_t
    auto start_time_t = std::mktime(&start_tm);
    auto end_time_t = std::mktime(&end_tm);
    
    for (const auto& book : book_info){
        std::tm book_start_tm = {};
        std::tm book_end_tm = {};
        std::istringstream book_start_ss(book.startTime);
        std::istringstream book_end_ss(book.endTime);
        
        book_start_ss >> std::get_time(&book_start_tm, "%Y-%m-%d %H:%M");
        book_end_ss >> std::get_time(&book_end_tm, "%Y-%m-%d %H:%M");
        
        auto book_start_time_t = std::mktime(&book_start_tm);
        auto book_end_time_t = std::mktime(&book_end_tm);
        
        if ((book_start_time_t < start_time_t && start_time_t < book_end_time_t) ||
            (book_start_time_t < end_time_t && end_time_t < book_end_time_t) ||
            (start_time_t < book_start_time_t && book_start_time_t < end_time_t) ||
            (start_time_t < book_end_time_t && book_end_time_t < end_time_t) ||
            book_start_time_t == start_time_t ||
            book_end_time_t == end_time_t
            ){
            return false;
        }
    }
    return true;
}

BookingSystem::BookingSystem(const std::string& username, const std::string& password) : curl_global_manager(),
    booker(std::string(), std::string(), std::string())
{
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

FieldEvent BookingSystem::retrieve_field_info(const std::string& field_name, const std::string& start_time, const std::string& end_time){
    if (field_dict.find(field_name) == field_dict.end()){
        std::cerr << "Field name not found in field_dict" << std::endl;
        return {"", "", {}, {}};
    }
    std::string field_id = field_dict.at(field_name);
    
    // URL编码参数
    char* escaped_field_id = curl_easy_escape(booking_handle, field_id.c_str(), static_cast<int>(field_id.length()));
    char* escaped_start_time = curl_easy_escape(booking_handle, start_time.c_str(), static_cast<int>(start_time.length()));
    char* escaped_end_time = curl_easy_escape(booking_handle, end_time.c_str(), static_cast<int>(end_time.length()));
    
    // 构建URL
    std::string url = "https://booking.cuhk.edu.cn/a/field/book/bizFieldBookField/eventsV1?ftId=" + 
                     std::string(escaped_field_id) + 
                     "&startTime=" + std::string(escaped_start_time) + 
                     "&endTime=" + std::string(escaped_end_time) + 
                     "&reBookMainId=&jsonStr=[]&fitUseStr=";
    
    // 释放编码后的字符串
    curl_free(escaped_field_id);
    curl_free(escaped_start_time);
    curl_free(escaped_end_time);
    
    std::string rawData = getRequest(url);
    FieldEvent field_event =  {field_id, field_name, _parse_field_info(rawData), get_field_places(field_id)};
    return field_event;
}

FieldEvent BookingSystem::retrieve_field_info_day(const std::string& field_name, const std::string& date_string){
    //将date_string转换为2024-11-25 00:00:00格式
    std::string start_time = date_string + " 07:00";
    std::string end_time = date_string + " 22:00";
    return retrieve_field_info(field_name, start_time, end_time);
}

std::string BookingSystem::get_available_time(const std::string& field_name, const std::string& date_string){
    if (!login())
    {
        return "Invalid username or password!";
    }
    FieldEvent field_event = retrieve_field_info_day(field_name, date_string);
    // 遍历早上7点到晚上10点的时间段
    std::map<std::string, std::vector<std::string>> available_time;
    for (int hour = 7; hour < 22; ++hour){
        std::string start_time = date_string + " " + std::to_string(hour) + ":00";
        std::string end_time = date_string + " " + std::to_string(hour + 1) + ":00";
        std::vector<std::string> places = field_event.is_available(start_time, end_time);
        if (!places.empty()){
            available_time[start_time + "--" + end_time] = places;
        }
    }
    std::string result;
    if (available_time.empty()){
        result = "No available time slots on " + date_string;
        return result;
    }
    result += "Available time slots: \n";
    for (const auto& time : available_time){
        result += time.first + "; ";
        result += "\n";
    }
    result += "\n";
    result += "Available places details: \n";
    for (const auto& time : available_time){
        result += time.first + "\n";
        for (const auto& place : time.second){
            result += "Place ID: " + place + ", Place Name: " + field_event.places.at(place) + "; \n";
        }
        result += "\n";
    }
    return result;
}

std::vector<BookEvent> BookingSystem::_parse_field_info(const std::string& raw_data){
    std::vector<BookEvent> info_list;
    
    // 解析JSON
    nlohmann::json json_data = nlohmann::json::parse(raw_data);

    // 处理event列表
    if(json_data.contains("event")) {
        for (const auto& event : json_data["event"]) {
            auto placeID = event["fId"].get<std::string>();
            if (placeID.empty()) continue;

            auto start_time_str = event["startTime"].get<std::string>();
            auto end_time_str = event["endTime"].get<std::string>();
            auto booker = event["userName"].get<std::string>();
            auto reason = event["theme"].get<std::string>();

            BookEvent book_event{
                start_time_str,
                end_time_str,
                placeID,
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
                start_time_str,
                end_time_str,
                placeID,
                booker,
                reason
            };
            info_list.push_back(book_event);
        }
    }

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

Booker::Booker(
    std::string  telephone,
    std::string  reason,
    std::string  details
) : telephone(std::move(telephone)), reason(std::move(reason)), details(std::move(details)){}

std::map<std::string, std::string> BookingSystem::_parse_info(const std::string& data) {
    std::map<std::string, std::string> info;
    
    xmlDocPtr doc = htmlReadMemory(data.c_str(), data.size(), NULL, "utf-8", HTML_PARSE_RECOVER|HTML_PARSE_NOERROR);
    if (doc == nullptr) {
        std::cerr << "Failed to parse HTML document\n";
        return info;
    }

    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    if (context == nullptr) {
        xmlFreeDoc(doc);
        return info;
    }

    // 定义需要获取的XPath表达式和对应的键名
    std::vector<std::pair<std::string, std::string>> xpath_queries = {
        {"//*[@id='id']/@value", "id"},
        {"//*[@id='userId']/@value", "userId"},
        {"//*[@id='userOrgId']/@value", "userOrgId"},
        {"//*[@id='bizFieldBookField.id']/@value", "bizFieldBookField.id"},
        {"//*[@id='bizFieldBookField.BId']/@value", "bizFieldBookField.BId"},
        {"//*[@id='userName']/@value", "userName"},
        {"//*[@id='userOrgName']/@value", "userOrgName"},
        {"//*[@id='userEmail']/@value", "userEmail"},
        {"//*[@id='bizFieldBookMainForm']/div/div/div/div[2]/div[6]/div/div/div/input/@value", "field_type"},
        {"//*[@id='bizFieldBookMainForm']/div/div/div/div[2]/div[7]/div/div/div/input/@value", "field_name"}
    };

    for (const auto& query : xpath_queries) {
        xmlXPathObjectPtr result = xmlXPathEvalExpression((const xmlChar*)query.first.c_str(), context);
        if (result && !xmlXPathNodeSetIsEmpty(result->nodesetval)) {
            xmlChar* value = xmlNodeGetContent(result->nodesetval->nodeTab[0]);
            if (value) {
                info[query.second] = reinterpret_cast<char*>(value);
                xmlFree(value);
            }
        }
        if (result) xmlXPathFreeObject(result);
    }

    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    return info;
}

std::map<std::string, std::string> BookingSystem::_get_info(
    const std::string& start_time,
    const std::string& end_time,
    const std::string& placeID
) const {
    std::string url = "https://booking.cuhk.edu.cn/a/field/client/bookForm";
    
    // URL编码参数
    char* escaped_start_time = curl_easy_escape(booking_handle, start_time.c_str(), static_cast<int>(start_time.length()));
    char* escaped_end_time = curl_easy_escape(booking_handle, end_time.c_str(), static_cast<int>(end_time.length()));
    char* escaped_placeID = curl_easy_escape(booking_handle, placeID.c_str(), static_cast<int>(placeID.length()));
    
    // 构建查询参数
    std::string params = "fId=" + std::string(escaped_placeID) +
                        "&bizFieldBookField.startTime=" + std::string(escaped_start_time) +
                        "&bizFieldBookField.endTime=" + std::string(escaped_end_time) +
                        "&repFlag=0&bookType=0&userTag=Student&approvalFlag=0" +
                        "&extend2=&bookedNum=0&fitBook=false&isDeptAdmin=false&adMost=1";
    
    // 释放编码后的字符串
    curl_free(escaped_start_time);
    curl_free(escaped_end_time);
    curl_free(escaped_placeID);
    
    url += "?" + params;
    
    // 使用 curl 发送 GET 请求
    Memory chunk = {nullptr, 0};
    curl_easy_setopt(booking_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(booking_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(booking_handle, CURLOPT_WRITEDATA, (void*)&chunk);
    
    CURLcode res = curl_easy_perform(booking_handle);
    std::string response;
    if (res == CURLE_OK && chunk.response) {
        response = chunk.response;
        free(chunk.response);
    }
    
    return _parse_info(response);
}

std::string BookingSystem::set_booker(const std::string& telephone, const std::string& reason, const std::string& details) {
    booker = Booker(telephone, reason, details);
    return "Booker information set successfully";
}

std::string BookingSystem::book(
    const FieldEvent& field_event,
    const std::string& placeID,
    const std::string& start_time,
    const std::string& end_time
) const
{
    if (!is_login) {
        std::cerr << "Please login before booking" << std::endl;
        return MESSAGE::LOGIN_REQUIRED;
    }
    if (booker.telephone.empty() || booker.reason.empty() || booker.details.empty()) {
        std::cerr << "Please set booker information before booking" << std::endl;
        return MESSAGE::BOOKER_INFO_REQUIRED;
    }
    auto info = _get_info(start_time, end_time, placeID);
    
    // URL编码所有参数值
    char* escaped_placeID = curl_easy_escape(booking_handle, placeID.c_str(), static_cast<int>(placeID.length()));
    char* escaped_reason = curl_easy_escape(booking_handle, booker.reason.c_str(), static_cast<int>(booker.reason.length()));
    char* escaped_field_id = curl_easy_escape(booking_handle, field_event.field_id.c_str(), static_cast<int>(field_event.field_id.length()));
    char* escaped_telephone = curl_easy_escape(booking_handle, booker.telephone.c_str(), static_cast<int>(booker.telephone.length()));
    char* escaped_start_time = curl_easy_escape(booking_handle, start_time.c_str(), static_cast<int>(start_time.length()));
    char* escaped_end_time = curl_easy_escape(booking_handle, end_time.c_str(), static_cast<int>(end_time.length()));
    char* escaped_details = curl_easy_escape(booking_handle, booker.details.c_str(), static_cast<int>(booker.details.length()));

    // 对info中的值进行编码
    std::map<std::string, std::string> escaped_info;
    for(const auto& pair : info) {
        char* escaped_value = curl_easy_escape(booking_handle, pair.second.c_str(), static_cast<int>(pair.second.length()));
        escaped_info[pair.first] = escaped_value;
        curl_free(escaped_value);
    }
    
    // 构建POST数据
    std::string post_data = 
        "id=" + escaped_info["id"] +
        "&user.id=" + escaped_info["userId"] +
        "&serialNo=" +
        "&userOrgId=" + escaped_info["userOrgId"] +
        "&status=" +
        "&approvalFlag=0" +
        "&bizFieldBookField.id=" + escaped_info["bizFieldBookField.id"] +
        "&bizFieldBookField.FId=" + std::string(escaped_placeID) +
        "&bizFieldBookField.BId=" + escaped_info["bizFieldBookField.BId"] +
        "&bizFieldBookField.theme=" + std::string(escaped_reason) +
        "&submitTime=" +
        "&isNewRecord=true" +
        "&extend1=" + std::string(escaped_field_id) +
        "&extend2=&extend3=&extend4=&extend5=" +
        "&userJob=" +
        "&userGrp=STUDENTS" +
        "&userMobile=" +
        "&bizFieldBookField.extend3=" +
        "&bizFieldBookField.extend4=" +
        "&bizFieldBookField.extend5=" +
        "&userTag=Student" +
        "&bookType=0" +
        "&fitBook=false" +
        "&user.name=" + escaped_info["userName"] +
        "&userOrgName=" + escaped_info["userOrgName"] +
        "&userEmail=" + escaped_info["userEmail"] +
        "&userPhone=" + std::string(escaped_telephone) +
        "&theme=" + std::string(escaped_reason) +
        "&bizFieldBookField.startTime=" + std::string(escaped_start_time) +
        "&bizFieldBookField.endTime=" + std::string(escaped_end_time) +
        "&bizFieldBookField.joinNums=2" +
        "&bizFieldBookField.needRep=0" +
        "&bizFieldBookField.extend1=0" +
        "&bizFieldBookField.useDesc=" + std::string(escaped_details);

    // 释放编码后的字符串
    curl_free(escaped_placeID);
    curl_free(escaped_reason);
    curl_free(escaped_field_id);
    curl_free(escaped_telephone);
    curl_free(escaped_start_time);
    curl_free(escaped_end_time);
    curl_free(escaped_details);

    std::string url = "https://booking.cuhk.edu.cn/a/field/book/bizFieldBookMain/saveData?reBookMainId=&ruleId=1249";
    
    Memory chunk = {nullptr, 0};
    curl_easy_setopt(booking_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(booking_handle, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(booking_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(booking_handle, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(booking_handle, CURLOPT_FOLLOWLOCATION, 1L);
    
    CURLcode res = curl_easy_perform(booking_handle);
    if (res != CURLE_OK) {
        throw std::runtime_error("Booking failed: " + std::string(curl_easy_strerror(res)));
    }

    if (chunk.response) {
        std::string response = chunk.response;
        free(chunk.response);
        
        nlohmann::json json_response = nlohmann::json::parse(response);
        if (!json_response["success"].get<bool>()) {
            return json_response["message"].get<std::string>();
        }
    }

    return MESSAGE::SUCCESS;
}

std::string BookingSystem::book_place(const std::string& field_name, const std::string& place_id, const std::string& start_time, const std::string& end_time){
    FieldEvent field_event = retrieve_field_info_day(field_name, start_time.substr(0, 10));
    const std::string res =  book(
        field_event,
        place_id,
        start_time,
        end_time
        );
    if (MESSAGE::SUCCESS == res) {
        return "Booking " + field_event.places.at(place_id) + " at " + start_time + " to " + end_time + " successful";
    }
    return "Booking " + field_event.places.at(place_id) + " at " + start_time + " to " + end_time + " failed";
}

std::string BookingSystem::book_field(const std::string& field_name, const std::string& start_time, const std::string& end_time){
    FieldEvent field_event = retrieve_field_info_day(field_name, start_time.substr(0, 10));
    std::vector<std::string> places = field_event.is_available(start_time, end_time);
    if (places.empty()) {
        std::string res = "No available places at " + start_time + " to " + end_time;
        return res;
    }
    std::string trial_log;
    for (const auto& place : places) {
        std::string res = book(field_event, place, start_time, end_time);
        if (res == MESSAGE::SUCCESS) {
            // return field_event.places.at(place);
            return "Booking " + field_event.places.at(place) + " at " + start_time + " to " + end_time + " successful";
        }
        trial_log += "Booking " + field_event.places.at(place) + " at " + start_time + " to " + end_time + " failed\n";
        trial_log += "Reason: " + res + "\n\n";
    }
    return trial_log;
}
