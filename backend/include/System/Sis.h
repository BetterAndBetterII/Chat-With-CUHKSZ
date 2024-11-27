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

    //command helper functions
    std::string getRequest(const std::string& url)const;

    std::string postRequest(const std::string& url, const std::string& strdata)const;

    std::vector<std::string> xpathQuery(const std::string& xmlContent, const std::string& xpathExpr)const;

    //接受学期作为参数，默认值为”2410UG“ 
    std::string get_course_id(const std::string& crouse)const;

    void recursive_search_assignments(const std::string& url, std::vector<std::string>& total_assignmnet_urls)const;

    std::string parse_assignment_url(const std::string& url)const;

    std::string vector_toString(const std::vector<std::string>& vector)const;
    
    //删除字符串前后多余空格和换行符
    std::string space_cutter(const std::string& str)const;

    //输入duedate和解析duedate的pattern判断due是否过期
    bool missDue(const std::string& content, const std::string& pattern)const;


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

    std::string get_announcement(const std::string& crouse, const int number = ELEM_TO_FIND ) const;
    /*参数：
     *crouse: 课程名称（例如 'CSC3001') 
     *number: 返回的annoucements数量（按新到旧依次输出）默认值为3
     *返回值:
     * 返回annoucement内容和发布者
    */

    std::string get_course(const std::string& term="2410UG") const;
    /*参数：
     *term：学期（例如‘2410UG’）格式：24（年份）10（学期：上学期10下学期20暑课30）UG（本科生）
     *返回值：
     *课程名
     *讲者
     *课程所属学期
    */
    
    std::string get_assignment(const std::string& crouse="ALL") const;
    /*参数：
     *crouse：课程名 默认为‘ALL’（但是推荐输入具体课程名）即返回所有课程（所有学期）的作业
     *返回值：  提交类作业    答题类作业
     *         作业名        作业名
     *         提交情况      Due
     *         Due
     *         分数
    */

    std::string get_grades(const std::string& crouse) const;
    /*参数：
     *crouse: 课程名称（例如 'CSC3001') 
     *number: 返回的annoucements数量（按新到旧依次输出）默认值为3
     *返回值:
     * 作业名称
     * 分数
     * 批改日期
    */
};


#endif //CHAT_WITH_CUHKSZ_SIS_H
