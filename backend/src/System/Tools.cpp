#include "../../include/System/Tools.h"
#include<iostream>
std::string Tools::handle_tool_call(const std::string& tool_name,  const json& arguments)
{
    json json_arguments = json::parse(std::string(arguments));
    if (tool_name == "generate_image"){
        return "Image generated!";
    }
    if (tool_name == "get_course") {
        return bb->get_course();
    }
    if (tool_name == "get_announcement") {
        return bb->get_announcement(std::string(json_arguments["class_id"]));
    }
    if (tool_name == "get_assignment") {
        return bb->get_assignment(std::string(json_arguments["class_id"]));
    }
    if (tool_name == "get_course_grades") {
        return bb->get_grades(std::string(json_arguments["class_id"]));
    }
    if (tool_name == "get_available_time") {
        return booking->get_available_time("badminton",std::string(json_arguments["date"]));
    }
    if (tool_name == "set_booker") {
        return booking->set_booker(std::string(json_arguments["telephone_number"]),std::string(json_arguments["reason"]),std::string(json_arguments["details"]));
    }
    if (tool_name == "book_field") {
        return booking->book_field("badminton",std::string(json_arguments["start_time"]),std::string(json_arguments["end_time"]));
    }
    if (tool_name=="search_library") {
        return library->search(std::string(json_arguments["Keyword"]),std::stoi(std::string(json_arguments["limit"])),std::string(json_arguments["tab"]));
    }
    if (tool_name=="get_schedule") {
        sis->login();
        return sis->get_schedule();
    }
    if (tool_name=="get_course") {
        sis->login();
        return sis->get_course(std::string(json_arguments["class_id"]));
    }
    if (tool_name=="get_term_grades") {
        sis->login();
        return sis->get_grades(std::string(json_arguments["term"]));
    }
    if (tool_name=="send_email") {
        std::string recipient_string=std::string(json_arguments["recipient"])+"@link.cuhk.edu.cn";
        std::vector<std::string> recipient_vector;
        for (char c : recipient_string) {
            recipient_vector.push_back(std::string(1, c));
        }
        return email->send_email(recipient_vector,std::string(json_arguments["subject"]),std::string(json_arguments["body"]));
    }
    // if (tool_name=="getKnowledge") {
    //     return knowledge->getKnowledge(std::string(arguments["keyword_1"]),std::string(arguments["keyword_2"]),std::string(arguments["keyword_3"]));
    // }
    return "Unknown tool!";
}
