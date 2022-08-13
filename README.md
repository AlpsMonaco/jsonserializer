# jsonserializer
This library aims to convert json string to C++ data struct in modern,fluent and fast way.  
Runtime safety and No C++ Excaption.

## Simple Example

### Map Like API
```c++
#include "json.h"
#include <string>
#include <vector>

const char* json_data = R"(
{
    "int" : 1,
    "object":{ "double" : 2.32}
    "string" : "hello world",
    "int_list":[1,2,3]
})";

int main(int argc,char**argv)
{
    jsr::Json json;
    auto err = json.Parse(json_data);
    if(err) return 1;
    auto dom = json.Deserialize();
    // All assignment are runtime safe,no exception even if the value not exist
    // return default value instead.
    int i = dom["i"];
    double d = dom["object"]["double"]; 
    std::string s = = dom["string"];
    std::vector<int> int_list = dom["int_list"];
}
```

### Reference Bind API
```c++
// same as Map Like API above
...
// if every key you registered in initializer_list exist and all types are matched,
// err will be false and value from json string will be set to you bind references appropriately.
int main(int argc,char**argv)
{
    int i; double d; 
    std::string s;
    std::vector<int> int_list;
    jsr::Json json;
    auto err = json.Parse(json_data);
    if(err) return 1;
    // register references in initializer_list,pass-by-reference
    err = json.Unmarshal({
        {"int",i},
        {"object":{
            {"double",d}}
        }
        {"string",s},
        {"int_list",int_list},
    })
    if(err) return 1;
}
```

