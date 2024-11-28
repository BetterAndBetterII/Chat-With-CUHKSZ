#include "../../include/System/Tools.h"
std::string Tools::handle_tool_call(const std::string& tool_name,  json& arguments)
{
    if (tool_name == "generate_image"){
        return "Image generated!";
    }
    if (tool_name == "get_course") {
        return bb->get_course();
    }
    if (tool_name == "get_announcement") {
        return bb->get_announcement(std::string(arguments["class_id"]));
    }
    if (tool_name == "get_available_time") {
        return booking->get_available_time("badminton",std::string(arguments["date"]));
    }
    if (tool_name == "set_booker") {
        booking->set_booker(std::string(arguments["telephone"]),std::string(arguments["reason"]),std::string(arguments["details"]));
        return "Booker set!";
    }
    if (tool_name == "book_field") {
        return booking->book_field(std::string(arguments["field_name"]),std::string(arguments["start_time"]),std::string(arguments["end_time"]));
    }
    return "Unknown tool!";
}
