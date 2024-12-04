#ifndef CHAT_WITH_CUHKSZ_SIS_H
#define CHAT_WITH_CUHKSZ_SIS_H

#include "../../include/System/CurlGlobal.h"
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h> 
#include <set>
#include <vector>

class SisSystem{
private:
    std::string username;
    std::string password;

    std::set<std::string> command_list;
    
    bool is_login;

    const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";//用于login生成随机字符串

    //cookiefile存储libcurl生成的cookie文件的路径
    std::string cookiefile;

    CURL* sis_handle;
    struct curl_slist* headers;
    //curlglobal类用于避免多次全局初始化
    CurlGlobal curl_global_manager;

    //Memory结构体用于存储请求返回的数据
    struct Memory {
    char* response;
    size_t size;
    };

    //callback functions用于处理返回内容
    static size_t ignore_calback(void *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t write_callback(char *data, size_t size, size_t nmemb, void *clientp);
    static size_t header_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userptr);

    //command helper functions
    std::string getRequest(const std::string& url)const;

    std::string postRequest(const std::string& url, const std::string& strdata)const;

    std::vector<std::string> xpathQuery(const std::string& xmlContent, const std::string& xpathExpr)const;

    std::string vector_toString(const std::vector<std::string>& vector)const;

    std::string course_parser(const std::string& courseinfo)const;

    void printTable(const std::vector<std::vector<std::string>>& Vector)const;

    std::string parse_ICSID(const std::string& text)const;

    std::string parse_ICStateNum(const std::string& text)const;
    
    //删除字符串前后多余空格和换行符
    std::string space_cutter(const std::string& str)const;

public:

    static const int ELEM_TO_FIND = 3;
    
    // 构造函数
    SisSystem(const std::string& username, const std::string& password);

    //析构函数
    ~SisSystem();

    //返回command_list
    std::string get_commands() const;

    //登录
    bool login();

    //初始化时若输入了错误的密码可以用change_info()来修改
    bool change_info(const std::string& username, const std::string& password);
    //若未登录成功(is_login == false)，则修改账号密码并返回true，若已登录则不会修改账号密码，返回false

    std::string get_schedule();
    /*
     *返回课表
    */

    std::string get_course(std::string course_code , std::string term="2410UG", std::string openOnly="Y");
    /*参数实例：
     *course_code: CSC3002
     *term: 2410UG(参考bb)
     *onpenOnly: Y/N (只查询开放状态的课程)
     *返回课程信息
     *!!有bug：不能返回全部内容
    */
    
    std::string get_grades(std::string term);
    /*参数格式实例：
     *2023-24 Term 1
     *2023-24 Term 2
     *2023-24 Summer Session
     *返回成绩
    */
};


#endif //CHAT_WITH_CUHKSZ_SIS_H
