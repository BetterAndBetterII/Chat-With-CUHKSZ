//
// Created by bette on 24-11-28.
//


#ifndef TOOLS_H
#define TOOLS_H

#include <nlohmann/json.hpp>
#include <utility>

#include "Blackboard.h"
#include "Booking.h"
#include "Libary.h"
#include "Sis.h"
#include "Email.h"
#include "../Knowledge/KnowledgeBase.h"
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
    LibarySystem *library=new LibarySystem();
    SisSystem *sis;
    EmailSystem *email;
    KnowledgeBase *knowledge=new KnowledgeBase;
public:
    Tools(const std::string &_username, const std::string &_password)
        : bb(new BlackBoardSystem(_username, _password)),
          booking(new BookingSystem(_username, _password)),
            sis(new SisSystem(_username, _password)),
            email(new EmailSystem(_username, _password)) {}
    ~Tools() {
        delete bb;
        delete booking;
        delete library;
        delete sis;
        delete email;
    };
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
                        {"class_id"}
            }
        },
        Function{
            "get_assignment",
            "Get the assignment of a class based on its class_id. Given the name of the class, you can get the class id from get_course function",
            FunctionParameters{
                            {
                                FunctionProperty{
                                    "class_id", "string", "The id of the class.",
                                }
                            },
                            {"class_id"}
            }
        },
        Function{
            "get_course_grades",
            "Get the grades of a class based on its class_id. Given the name of the class, you can get the class id from get_course function",
            FunctionParameters{
                            {
                                FunctionProperty{
                                    "class_id", "string", "The id of the class.",
                                }
                            },
                            {"class_id"}
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
                            {"date"}
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
        Function{
            "search_library",
            "search for print_books, journals, articles, ebook in the library.",
            FunctionParameters{
                                {
                                    FunctionProperty{
                                        "Keyword", "string", "The keyword used for searching",
                                    },
                                    FunctionProperty{
                                        "limit", "string", "The number of results the search should return",
                                    },
                                    FunctionProperty{
                                        "tab", "string", "the tab of the resource","Everything",{"Everything","PrintBooks/Journals","Articles/eBooks"}
                                    },
                                },
                                {"Keyword","limit","tab"}
            }
        },
        Function{
            "get_schedule",
            "get the schedule of the user",
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
            "get_course",
            "get the detail information of a course based on its class id",
            FunctionParameters{
                                {
                                    FunctionProperty{
                                        "class_id", "string", "The id of the class.",
                                    }
                                },
                                {"class_id"}
            }
        },
        Function{
            "get_term_grades",
            "get the grade of the user based on the term",
            FunctionParameters{
                                {
                                    FunctionProperty{
                                        "term", "string", "The term for searching the grade e.g.2023-24 Term 2.",
                                    }
                                },
                                {"term"}
            }
        },
        Function{
            "send_email",
            "send a email to one or more recipients with identifier without domain",
            FunctionParameters{
                                    {
                                        FunctionProperty{
                                            "recipients", "string", "The recipients of the email without domain, separated by space. You should show the email to the user first before sending the email",
                                        },
                                        FunctionProperty{
                                            "subject", "string", "The subject of the emial",
                                        },
                                        FunctionProperty{
                                            "body", "string", "the main body of th email",
                                        },
                                    },
                                    {"recipient","subject","body"}
            }
        },
        Function{
            "getKnowledge",
            "pass three key words as parameters to perform search in the knowledge base of CUHKSZ. You MUST search the knowledge base when the question is relevant to CUHKSZ",
            FunctionParameters{
                                    {
                                        FunctionProperty{
                                            "keyword_1", "string", "The most relevant key word.",
                                        },
                                        FunctionProperty{
                                            "keyword_2", "string", "The second relevant key word",
                                        },
                                        FunctionProperty{
                                            "keyword_3", "string", "The third relevant key word",
                                        },
                                    },
                                    {"keyword_1","keyword_2","keyword_3"}
            }
        },
    };
    std::string handle_tool_call(const std::string &tool_name, const json& arguments);
};

#endif // TOOLS_H
