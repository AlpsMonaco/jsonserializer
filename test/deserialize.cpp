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
    jsr::Json json;
    auto err = json.Parse(sample_data);
    HandleError(err);
    jsr::Document dom = json.Deserialize();
    for (auto it = dom["int"].Begin(); it != dom["int"].End();)
    {
        std::cout << it->name.GetString() << std::endl;
        it++;
    }
    return 0;
    int i = dom["int"];
    double d = dom["double"];
    std::int64_t i64 = dom["int64"];
    std::string s = dom["string"];
    bool b = dom["bool"];
    std::vector<int> int_list = dom["int_array"];
    std::vector<std::string> string_list = dom["string_array"];
    int object_i = dom["object"]["int"];
    double object_d = dom["object"]["double"];
    const char* object_const_char_ptr = dom["object"]["const_char_ptr"];
    std::string_view sv = dom["object"]["object"]["string_view"];
    jsr::Array array = dom["object"]["object"]["multi_type_array"];
    int array_int = array[0];
    double array_double = array[1];
    std::string array_string = array[2];
    std::cout << i << std::endl;
    std::cout << d << std::endl;
    std::cout << i64 << std::endl;
    std::cout << s << std::endl;
    std::cout << b << std::endl;
    for (const auto& v : int_list)
        std::cout << v << std::endl;
    for (const auto& v : string_list)
        std::cout << v << std::endl;
    std::cout << object_i << std::endl;
    std::cout << object_d << std::endl;
    std::cout << object_const_char_ptr << std::endl;
    std::cout << sv << std::endl;
    std::cout << array_int << std::endl;
    std::cout << array_double << std::endl;
    std::cout << array_string << std::endl;
    return 0;
}