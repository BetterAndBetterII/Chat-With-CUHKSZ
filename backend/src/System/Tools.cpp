#include "../../include/System/Tools.h"
#include<iostream>
std::string Tools::handle_tool_call(const std::string& tool_name,  const json& arguments)
{
    json json_arguments = json::parse(std::string(arguments));
    // if (tool_name == "generate_image"){
    //     return "Image generated!";
    // }
    if (tool_name == "get_course_list") {
        return bb->get_course();
    }
    if (tool_name == "get_course_announcement") {
        return bb->get_announcement(std::string(json_arguments["course_id"]));
    }
    if (tool_name == "get_course_assignment") {
        return bb->get_assignment(std::string(json_arguments["course_id"]));
    }
    if (tool_name == "get_course_grades") {
        return bb->get_grades(std::string(json_arguments["course_id"]));
    }
    if (tool_name == "get_badminton_court_available_time") {
        return booking->get_available_time("badminton",std::string(json_arguments["date"]));
    }
    if (tool_name == "set_badminton_field_booker") {
        return booking->set_booker(std::string(json_arguments["telephone_number"]),std::string(json_arguments["reason"]),std::string(json_arguments["details"]));
    }
    if (tool_name == "book_badminton_field") {
        return booking->book_field("badminton",std::string(json_arguments["start_time"]),std::string(json_arguments["end_time"]));
    }
    if (tool_name=="search_library_resource") {
        return library->search(std::string(json_arguments["Keyword"]),std::stoi(std::string(json_arguments["limit"])),std::string(json_arguments["tab"]));
    }
    if (tool_name=="get_schedule") {
        sis->login();
        return sis->get_schedule();
    }
    if (tool_name=="get_course_information") {
        sis->login();
        return sis->get_course(std::string(json_arguments["course_id"]));
    }
    if (tool_name=="get_term_grades") {
        sis->login();
        return sis->get_grades(std::string(json_arguments["term"]));
    }
    if (tool_name=="send_email") {
        std::string recipient_string=std::string(json_arguments["recipients"]);
        std::vector<std::string> recipient_vector;
        std::istringstream stream(recipient_string);
        std::string parameter;
        while (stream >> parameter) {
            parameter += "@link.cuhk.edu.cn";
            recipient_vector.push_back(parameter);
        }
        return email->send_email(recipient_vector,std::string(json_arguments["subject"]),std::string(json_arguments["body"]));
    }
    if (tool_name=="getKnowledge") {
        knowledge->loadAllFiles("/home/yf/Workplace/Group_project/Chat-With-CUHKSZ/backend/KnowledgeBase/phoenix_cuhksz_knowledge-main");
        return knowledge->getKnowledge(std::string(json_arguments["keyword_1"]),std::string(json_arguments["keyword_2"]),std::string(json_arguments["keyword_3"]));
    }
    return "Unknown tool!";
}
