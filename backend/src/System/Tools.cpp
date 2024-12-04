#include "../../include/System/Tools.h"
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
    if (tool_name == "get_available_time") {
        return booking->get_available_time("badminton",std::string(json_arguments["date"]));
    }
    if (tool_name == "set_booker") {
        return booking->set_booker(std::string(json_arguments["telephone_number"]),std::string(json_arguments["reason"]),std::string(json_arguments["details"]));
    }
    if (tool_name == "book_field") {
        return booking->book_field("badminton",std::string(json_arguments["start_time"]),std::string(json_arguments["end_time"]));
    }
    // if (tool_name=="getKnowledge") {
    //     return knowledge->getKnowledge(std::string(arguments["keyword_1"]),std::string(arguments["keyword_2"]),std::string(arguments["keyword_3"]));
    // }
    return "Unknown tool!";
}
