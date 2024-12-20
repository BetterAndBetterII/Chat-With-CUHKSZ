//
// Created by Gary on 24-10-24.
// Complemented by Wen on 24-11-18.
//

#ifndef CHAT_WITH_CUHKSZ_BLACKBOARD_H
#define CHAT_WITH_CUHKSZ_BLACKBOARD_H

#include "../../include/System/System.h"

class BlackBoardSystem : public System{
private:

    //接受学期作为参数，默认值为”2410UG“ 
    std::string get_course_id(const std::string& crouse)const;

    void recursive_search_assignments(const std::string& url, std::vector<std::string>& total_assignmnet_urls)const;

    std::string parse_assignment_url(const std::string& url)const;

    //删除字符串前后多余空格和换行符
    std::string space_cutter(const std::string& str)const;

    //输入duedate和解析duedate的pattern判断due是否过期
    bool missDue(const std::string& content, const std::string& pattern)const;

public:

    static const int ELEM_TO_FIND = 3;
    
    // 构造函数
    BlackBoardSystem(const std::string& username, const std::string& password);

    //析构函数
    ~BlackBoardSystem();

    //登录
    bool login();

    std::string get_announcement(const std::string& crouse, const int number = ELEM_TO_FIND );
    /*参数：
     *crouse: 课程名称（例如 'CSC3001') 
     *number: 返回的annoucements数量（按新到旧依次输出）默认值为3
     *返回值:
     * 返回annoucement内容和发布者
    */

    std::string get_course(const std::string& term="2410UG");
    /*参数：
     *term：学期（例如‘2410UG’）格式：24（年份）10（学期：上学期10下学期20暑课30）UG（本科生）
     *返回值：
     *课程名
     *讲者
     *课程所属学期
    */
    
    std::string get_assignment(const std::string& crouse="ALL");
    /*参数：
     *crouse：课程名 默认为‘ALL’（但是推荐输入具体课程名）即返回所有课程（所有学期）的作业
     *返回值：  提交类作业    答题类作业
     *         作业名        作业名
     *         提交情况      Due
     *         Due
     *         分数
    */

    std::string get_grades(const std::string& crouse);
    /*参数：
     *crouse: 课程名称（例如 'CSC3001') 
     *number: 返回的annoucements数量（按新到旧依次输出）默认值为3
     *返回值:
     * 作业名称
     * 分数
     * 批改日期
    */

    std::string get_name();
    //返回姓名
};


#endif //CHAT_WITH_CUHKSZ_BLACKBOARD_H
