//
// Created by bette on 24-11-28.
//


#ifndef TOOLS_H
#define TOOLS_H

#include <nlohmann/json.hpp>
#include <utility>

#include "Blackboard.h"
#include "Booking.h"
using json = nlohmann::json;
class FunctionProperty
{
public:
    std::string name;
    std::string type;
    std::string description;
    std::string default_value;
    std::vector<std::string> enum_values;

    FunctionProperty(std::string name,
                     std::string type,
                     std::string description,
                     std::string default_value = "",
                     std::vector<std::string> enum_values = {})
        : name(std::move(name)),
          type(std::move(type)),
          description(std::move(description)),
          default_value(std::move(default_value)),
          enum_values(std::move(enum_values))
    {
    }
};

class FunctionParameters
{
public:
    std::string type = "object";
    std::vector<FunctionProperty> properties;
    std::vector<std::string> required;

    FunctionParameters(std::vector<FunctionProperty> properties, std::vector<std::string> required)
        : properties(std::move(properties)), required(std::move(required)){}
    FunctionParameters() = default;
};

class FunctionDescription
{
public:
    std::string name;
    std::string description;
    FunctionParameters parameters;

    FunctionDescription(std::string name, std::string description, FunctionParameters parameters)
        : name(std::move(name)), description(std::move(description)), parameters(std::move(parameters))
    {
    }
};

class Function
{
public:
    std::string type = "function";
    FunctionDescription description;

    Function(std::string name, std::string description, FunctionParameters parameters={})
        : description(std::move(name), std::move(description), std::move(parameters))
    {
    }

    [[nodiscard]] json to_json() const
    {
        json properties_obj;
        // 为每个 property 创建 JSON 对象
        for (const auto& prop : description.parameters.properties)
        {
            json prop_obj = {
                {"type", prop.type},
                {"description", prop.description}
            };

            // 只有在有默认值时才添加
            if (!prop.default_value.empty())
            {
                prop_obj["default"] = prop.default_value;
            }

            // 只有在有枚举值时才添加
            if (!prop.enum_values.empty())
            {
                prop_obj["enum"] = prop.enum_values;
            }

            properties_obj[prop.name] = prop_obj;
        }

        return {
            {"type", type},
            {
                "function",
                {
                    {"name", description.name}, {"description", description.description},
                    {
                        "parameters",
                        {
                            {"type", description.parameters.type}, {"properties", properties_obj},
                            {"required", description.parameters.required}
                        }
                    }
                }
            }
        };
    }
};

class Tools
{
    BlackBoardSystem *bb;
    BookingSystem *booking;
    // KnowledgeBase *knowledge;
public:
    Tools(const std::string &_username, const std::string &_password)
        : bb(new BlackBoardSystem(_username, _password)),
          booking(new BookingSystem(_username, _password)){}
    ~Tools();
    std::vector<Function> functions = {
        Function{
            "generate_image",
            "Generate an image based on the provided description using DALL·E 3.",
            FunctionParameters{
                {
                    FunctionProperty{"prompt", "string", "A description of the image to generate."},
                    FunctionProperty{
                        "size", "string", "The size of the image to generate.", "1024x1024",
                        {"1024x1024", "1792x1024", "1024x1792"}
                    }
                },
                {"prompt"}
            }
        },
        Function{
            "get_course",
            "Get the course of the user",
            FunctionParameters{
                    {
                        FunctionProperty{
                            "placeholder", "string", "The placeholder of the parameter.",
                        }
                    },
                    {}
            }
        },
        Function{
            "get_announcement",
            "Get the announcement of a class based on its class_id. Given the name of the class, you can get the class id from get_course function",
            FunctionParameters{
                        {
                            FunctionProperty{
                                "class_id", "string", "The id of the class.",
                            }
                        },
                        {}
            }
        },
        Function{
            "get_available_time",
            "Get the available time of the badminton court based on a date",
            FunctionParameters{
                            {
                                FunctionProperty{
                                    "date", "string", "The date of the query with the format %Y-%m-%d",
                                }
                            },
                            {}
            }
        },
        Function{
            "set_booker",
            "a function that must be call before calling book_field function. You must explicitly ask user for the telephone number, reason and details before calling this function",
            FunctionParameters{
                            {
                                FunctionProperty{
                                    "telephone_number", "string", "The telephone number of the user.",
                                },
                                FunctionProperty{
                                    "reason", "string", "The reason for the reservation.",
                                },
                                FunctionProperty{
                                    "details", "string", "The detail of the reservation.",
                                }
                            },
                            {"telephone_number", "reason", "details"}
            }
        },
        Function{
            "book_field",
            "Book a badminton court. The booking period should NOT longer than 1 hour! You must call set_booker function before calling this function.",
            FunctionParameters{
                            {
                                FunctionProperty{
                                    "start_time", "string", "The start time of the reservation with the format %Y-%m-%d %H:%M",
                                },
                                FunctionProperty{
                                    "end_time", "string", "The end time of the reservation with the format %Y-%m-%d %H:%M",
                                },
                            },
                            {"start_time", "end_time"}
            }
        },
        // Function{
        //     "getKnowledge",
        //     "pass three key words as parameters to perform search in the knowledge base of  the university",
        //     FunctionParameters{
        //                         {
        //                             FunctionProperty{
        //                                 "keyword_1", "string", "The most relevant key word.",
        //                             },
        //                             FunctionProperty{
        //                                 "keyword_2", "string", "The second relevant key word",
        //                             },
        //                             FunctionProperty{
        //                                 "keyword_3", "string", "The third relevant key word",
        //                             },
        //                         },
        //                         {}
        //     }
        // },
    };

    std::string handle_tool_call(const std::string &tool_name, const json& arguments);
};

#endif // TOOLS_H
