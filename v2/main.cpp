#include <iostream>
#include "value.h"

int main(int argc, char** argv)
{
    rapidjson::Document d;
    const char* json = R"(
    {
        "object":{
            "i":["foo","bar"]
        }        
    }
    )";
    d.Parse(json);
    jsr::Value value(d);
    std::vector<std::string> s_list = value["object"]["i"];
    for (auto v : s_list)
    {
        std::cout << v << std::endl;
    }
}