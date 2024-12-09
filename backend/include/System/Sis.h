#ifndef CHAT_WITH_CUHKSZ_SIS_H
#define CHAT_WITH_CUHKSZ_SIS_H

#include "../../include/System/System.h"

class SisSystem : public System{
private:

    const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";//用于login生成随机字符串

    std::string course_parser(const std::string& courseinfo)const;

    //测试get_schedule table 爬取
    void printTable(const std::vector<std::vector<std::string>>& Vector)const;

    std::string parse_ICSID(const std::string& text)const;

    std::string parse_ICStateNum(const std::string& text)const;
    
public:

    static const int ELEM_TO_FIND = 3;
    
    // 构造函数
    SisSystem(const std::string& username, const std::string& password);

    //析构函数
    ~SisSystem();

    //登录
    bool login();

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
