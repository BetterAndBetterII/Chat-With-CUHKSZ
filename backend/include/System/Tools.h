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
    LibarySystem *library;
    SisSystem *sis;
    EmailSystem *email;
    KnowledgeBase *knowledge=new KnowledgeBase();
public:
    Tools(const std::string &_username, const std::string &_password)
        : bb(new BlackBoardSystem(_username, _password)),
          booking(new BookingSystem(_username, _password)),
            sis(new SisSystem(_username, _password)),
            email(new EmailSystem(_username, _password)),
            library(new LibarySystem(_username, _password)){}
    ~Tools() {
        delete bb;
        delete booking;
        delete library;
        delete sis;
        delete email;
    };
    std::vector<Function> functions = {
        // Function{
        //     "generate_image",
        //     "Generate an image based on the provided description using DALL·E 3.",
        //     FunctionParameters{
        //         {
        //             FunctionProperty{"prompt", "string", "A description of the image to generate."},
        //             FunctionProperty{
        //                 "size", "string", "The size of the image to generate.", "1024x1024",
        //                 {"1024x1024", "1792x1024", "1024x1792"}
        //             }
        //         },
        //         {"prompt"}
        //     }
        // },
        Function{
            "get_course_list",
            "Get the course list of the user",
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
            "get_course_announcement",
            "Get the announcement of a course based on its course_id. Given the name of the course, you can get the course_id from the get_course_list function",
            FunctionParameters{
                        {
                            FunctionProperty{
                                "course_id", "string", "The id of the course.",
                            }
                        },
                        {"course_id"}
            }
        },
        Function{
            "get_course_assignment",
            "Get the assignment of a course based on its course_id. Given the name of the course, you can get the course_id from get_course_list function",
            FunctionParameters{
                            {
                                FunctionProperty{
                                    "course_id", "string", "The id of the class.",
                                }
                            },
                            {"course_id"}
            }
        },
        Function{
            "get_course_grades",
            "Get the grades of a course based on its course_id. Given the name of the course, you can get the course_id from get_course_list function",
            FunctionParameters{
                            {
                                FunctionProperty{
                                    "course_id", "string", "The id of the class.",
                                }
                            },
                            {"course_id"}
            }
        },
        Function{
            "get_badminton_court_available_time",
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
            "set_badminton_field_booker",
            "a function that MUST be call before calling book_badminton_field function. After calling this function, you MUST call book_badminton_field function to book a field according to the requirement of the book_badminton_field function",
            FunctionParameters{
                            {
                                FunctionProperty{
                                    "telephone_number", "string", "The telephone number of the user.",
                                },
                                FunctionProperty{
                                    "reason", "string", "The reason for the reservation, you can fill in by yourself.",
                                },
                                FunctionProperty{
                                    "details", "string", "The detail of the reservation, you can fill in by yourself.",
                                }
                            },
                            {"telephone_number", "reason", "details"}
            }
        },
        Function{
            "book_badminton_field",
            "Book a badminton field. The booking period should NOT longer than 1 hour! You can only book the field of today and tomorrow! You MUST call set_badminton_field_booker function before calling this function. You MUST call this function after calling set_badminton_field_booker function",
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
            "search_library_resource",
            "search for print_books, journals, articles, ebook in the library.",
            FunctionParameters{
                                {
                                    FunctionProperty{
                                        "Keyword", "string", "The keyword used for searching",
                                    },
                                    FunctionProperty{
                                        "limit", "string", "The number of results the search should return, you can determine by yourself",
                                    },
                                    FunctionProperty{
                                        "tab", "string", "the tab of the resource, you can determine by yourself","Everything",{"Everything","PrintBooks/Journals","Articles/eBooks"}
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
            "get_course_information",
            "get the detail information of a course based on its course_id. Given the name of the course, you can get the course_id from the get_course_list function",
            FunctionParameters{
                                {
                                    FunctionProperty{
                                        "course_id", "string", "The id of the course.",
                                    }
                                },
                                {"course_id"}
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
            "send a email to one or more recipients with identifier without domain. You should show the email to the user first before sending the email. If the email is not formal style, you can use emoji to prettify the email, and add one in the beginning of subject.",
            FunctionParameters{
                                    {
                                        FunctionProperty{
                                            "recipients", "string", "The recipients of the email without domain, separated by space. IF you do NOT know the recipient, you MUST ask the user for that. You MUST NOT leave it blank and MUST NOT represent it as your_email. Specially, the user's self email is the username of the user.",
                                        },
                                        FunctionProperty{
                                            "subject", "string", "The subject of the email, you can determine by yourself",
                                        },
                                        FunctionProperty{
                                            "body", "string", "the main body of th email, you can determine by yourself. You should use html format",
                                        },
                                    },
                                    {"recipient","subject","body"}
            }
        },
        Function{
            "getKnowledge",
            "pass three key words as parameters to perform search in the knowledge base of CUHKSZ(港中深，香港中文大学深圳）, which is the university of the user. You MUST search the knowledge base when the question is relevant to CUHKSZ, such as getting information or introducing about the professor or the campus. You MUST use Chinese as key word except for searching for the information of a foreign professor. Some name of the Chinese professor is in pinyin so you MUST identify them and translate them into Chinese character. For example, you MUST transfer Rui Huang to 黄锐",
            FunctionParameters{
                                    {
                                        FunctionProperty{
                                            "keyword_1", "string", "The most relevant key word. IF you are asking for the information of a professor, his or her name should be the most relevant keyword",
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

    bool is_valid_login(const std::string& username, const std::string& password);

    std::string get_name(const std::string& username, const std::string& password);
};

#endif // TOOLS_H
