//
// Created by bette on 24-11-28.
//


#ifndef TOOLS_H
#define TOOLS_H

#include <nlohmann/json.hpp>
#include <utility>
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
        : properties(std::move(properties)), required(std::move(required))
    {
    }
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

    Function(std::string name, std::string description, FunctionParameters parameters)
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
public:
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
        }
    };

    static std::string handle_tool_call(const std::string& tool_name, const json& arguments)
    {
        if (tool_name == "generate_image")
        {
            return "Image generated!";
        }
        return "Unknown tool!";
    }
};

#endif // TOOLS_H
