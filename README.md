# jsonserializer
This library aims to convert json string to C++ data struct in modern,fluent and fast way.  
Runtime safety and No C++ Exception.

## Simple Example

### Reference Bind API
```c++
#include "jsonserializer/json.h"
#include <string>
#include <vector>

const char* json_data = R"(
{
    "int" : 1,
    "object":{ "double" : 2.32},
    "string" : "hello world",
    "int_list":[1,2,3]
})";

// if every key you registered in the initializer_list exist and all types are matched,
// err will be false and value from json string will be set to you bind references appropriately.
int main(int argc,char**argv)
{
    int i; double d; 
    std::string s;
    std::vector<int> int_list;
    jsr::Json json;
    auto err = json.Parse(json_data);
    if(err) return 1;
    // register references in a initializer_list,pass-by-reference
    err = json.Unmarshal({
        {"int",i},
        {"object",{
            {"double",d}}
        },
        {"string",s},
        {"int_list",int_list},
    });
    if(err) return 1;
}
```

### Map Like API
```c++
...
const char* json_data = R"(
{
    "int" : 1,
    "object":{ "double" : 2.32},
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
    int i = dom["int"];
    double d = dom["object"]["double"]; 
    std::string s = dom["string"];
    std::vector<int> int_list = dom["int_list"];
}
```

## Value

### Basic Type
This lib treats `number,bool,double,string` as basic type and they could be assigned to c++  
data struct directly through `Reference Bind API` or `Map Like API`  
```c++
const char * sample = R"({ "i" : 1 })";
...
jsr::Json json;
json.Parse(sample);

// Map Like API
auto d = json.Deserialize();
int i = d["i"];

// Reference Bind API
int j;
auto err = json.Unmarshal({{"i",j}});
```

### Array With Basic Type
A json array holds basic type could be assigned to `std::vector` directly as well.
```c++
const char * sample = R"({ "s_list" : ["foo","bar"] })";
...
jsr::Json json;
json.Parse(sample);

// Map Like API
auto d = json.Deserialize();
std::vector<std::string> s_list = d["s_list"];

// Reference Bind API
std::vector<std::string> rs_list;
auto err = json.Unmarshal({{"s_list",rs_list}});
```

### Array With Multi Type
A json array could holds values of diffierent types and it is much tricky.You could use `jsr::Array`  
to resolve it.It is runtime safety class and easy to use.  
```c++
const char * sample = R"({ "s_list" : [1,"foo",false,{"foo":"bar"}] })";
...
jsr::Json json;
json.Parse(sample);

// Map Like API
auto d = json.Deserialize();
jsr::Array array = d["s_list"];
int i = array[0];
std::string s = array[1];
bool b = array[2];
double d = array[3]; // also safe,return 0.0 

// Reference Bind API
jsr::Array rarray;
auto err = json.Unmarshal({
    {"s_list",rarray}
});
int ri = rarray[0];
std::string rs = rarray[1];
bool rb = rarray[2];

// you could Unmarshal to a binded reference through initializer_list too.
std::string foo;
auto err = rarray[3].Unmarshal({{"foo",foo}});
```

### Object Type
A json object could holds multi objects with diffierent types.You could assigment them  
through `Reference Bind API` or `Map Like API`  
```c++
const char * sample = R"({"i":1,"s":"string","d":3.31})";
struct Foo{
    int i;
    std::string s;
    double d;
};
jsr::Json json;
json.Parse(sample);

// Map Like API
auto d = json.Deserialize();
Foo foo;
foo.i = d["i"];
foo.s = d["s"];
foo.d = d["d"];

// Reference Bind API
Foo foo2;
auto err = json.Unmarshal({
    {"i",foo2.i},
    {"s",foo2.s},
    {"d",foo2.d},
});
if (err) return 1;

```


## API Rule

### Deserialize
`jsr::Json::Deserialize` returns a `jsr::Document` object.  
You could fetch value and assign to C++ data struct from any json string through string literal using `operator[]`.
```c++
... 
auto d = json.Deserailize();
std::string s = d["string"];
int i = d["score"];
...
```
This API provides runtime safety assgment thats means every assigment will be successful  
certainly and no Exception will be raised even a key you use to fetch value from a jsr::Document object  
already parsed json string is not exists.  

### Marshal
`jsr::Json::Marshal` requires a initializer_list (`jsr::Object` container) to  
resolve json string and bind value from json string to C++ data sturct.  
```c++
... 
std::string s;
int i;
auto err = json.Unmarshal({
    {"i",i},
    {"s",s},
});
...
```
This API has a very strict map rule to resolve json string.  
Every keys you indicates from a initializer_list must exist from  
json string you parse and the types are all matched from both json string and C++ data struct.  

```c++
const char* sample = R"({"foo":"bar"})";
...
jsr::Json json;
auto err = json.Parse(sample);

int i;
std::string s;
auto err = json.Unmarshal({{"bar",i}}); // err == true,'bar' not exist in sample.
err = json.Unmarshal({{"foo",i}}); // err == true,foo.bar from sample is a string,not a int.
err = json.Unmarshal({{"foo",s}}); // err == false,ok! "bar" is assigment to s successfully.
```


## Nested Object
Nested Object is supported.**Just make sures the object level is matched from  
both json string and `jsr` container( `jsr::Document` or `jsr::Object`)**


## Third Party

### rapidjson
Thanks to `https://github.com/Tencent/rapidjson`.
`rapidjson` as internal json parse library which is SUPER fast.  
This lib is also pass-by-reference in most conditions to reduce construction.  

This lib doesn't use git submodule to maintaince `rapidjson` dependency due to poor network  
from China To GitHub Server.  
