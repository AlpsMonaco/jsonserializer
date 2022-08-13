#include <iostream>
#include <iomanip>
#include "jsonserializer/json.h"

#define HandleError(err)      \
    do                        \
    {                         \
        if (err)              \
        {                     \
            std::cout << err; \
            return 1;         \
        }                     \
    } while (0)

const char* sample_data = R"(
{
    "int": 1,
    "double": 3.1415926,
    "int64": 922337203685,
    "string": "string",
    "bool": false,
    "int_array":[1,2,3,4,5],
    "string_array":["hello","world"],
    "object":{
        "int": 1,
        "double": 3.1415926,
        "const_char_ptr":"object.const_char_ptr",
        "object":{
            "string_view":"object.string_view",
            "multi_type_array":[934,1.535,"multi_type_array"]
        }
    }
}
)";

struct Object
{
    int i;
    double d;
    const char* s;
};

int main(int argc, char** argv)
{
    int i;
    double d;
    std::int64_t i64;
    std::string s;
    bool b;
    std::vector<int> int_list;
    std::vector<std::string> string_list;
    Object object;
    std::string_view sv;
    jsr::Array array;
    jsr::Json json;
    auto err = json.Parse(sample_data);
    HandleError(err);
    err = json.Unmarshal(
        {
            {"int", i},
            {"double", d},
            {"int64", i64},
            {"string", s},
            {"bool", b},
            {"int_array", int_list},
            {"string_array", string_list},
            {"object", 
                {
                    {"int", object.i}, 
                    {"double", object.d}, 
                    {"const_char_ptr", &object.s},
                    {"object", {
                        {"string_view", sv},
                        {"multi_type_array", array}}
                    }
                }
            }
        });
    HandleError(err);
    std::cout << std::setprecision(10);
    std::cout << i << std::endl;
    std::cout << d << std::endl;
    std::cout << i64 << std::endl;
    std::cout << s << std::endl;
    std::cout << b << std::endl;
    for (const auto& v : int_list)
        std::cout << v << std::endl;
    for (const auto& v : string_list)
        std::cout << v << std::endl;
    std::cout << object.i << std::endl;
    std::cout << object.d << std::endl;
    std::cout << object.s << std::endl;
    std::cout << sv << std::endl;
    const int& ri = array[0];
    const double& rd = array[1];
    const std::string& rs = array[2];
    std::cout << ri << std::endl;
    std::cout << rd << std::endl;
    std::cout << rs << std::endl;
    return 0;
}