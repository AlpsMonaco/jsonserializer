# jsonserializer
a strong struct-json value bind serializer/unserializer lib,simple api and strong check.

## Simple Usage
you could construct a bind map through initilizer_list;
```
#include <iostream>
#include "jsonserializer.h"

using namespace std;
using namespace jsonserializer;

struct Foo
{
    int i;
    std::string s;
};

const char* json = R"(
{
    "i":1,
    "s":"hello world"
})";

int main(int argc,char**argv)
{
    JsonSerializer js;
    if (!js.Parse(json)) {cout << "parse error" << endl;return 1;}
    Foo foo;
    auto err = js.Unseralize(
    {
        {"i",&foo.i},
        {"s",&foo.s}
    }
    );
    if(err) {cout << err ;return 1;}
    cout << foo.i << endl;
    cout << foo.s << endl;
}
```
## Introduction

### Basic Rules
This library requires fully matched between a object of a C++ struct and the json string.  
Keys must be exist in the json string and the type of the value must be the same  
on both fields of the struct and json string.  
for example:
```
jsonserializer jsr;
const char* json = R"({"i":1})";
jsr.Parse(json);
std::string s;
int i;
auto err = jsr.Unserialier({{"i",&s}}); // error! type mismatch.
err = jsr.Unserialier({{"int",&i}}); // error! key not exist.
```


### Nested Object
This library also support nested object to be parsed.
```
#include <iostream>
#include "jsonserializer.h"
using namespace std;
using namespace jsonserializer;
struct Foo{
    int i;
    std::string s;
}

struct Bar{
    int j;
    Foo foo;
}

const char* json = R"(
{
    "j":1,
    "foo":
    {
        "i":2,
        "s":"hello world"
    }
})";

int main(int argc,char**argv)
{
    JsonSerializer js;
    if (!js.Parse(json)) {cout << "parse error" << endl;return 1;}
    Bar bar;
    auto err = js.Unseralize(
    {
        {"j",&bar.j},
        {"foo",{
            {
                "i":&bar.foo.i,
                "s":&bar.foo.s,
            }
        }
        }
    }
    );
    if(err) {cout << err ;return 1;}
    cout << bar.j << endl;
    cout << bar.foo.i << endl;
    cout << bar.foo.s << endl;
}
```


### Array Convert
We use std::vector as the container to hold json values.  

#### Simple Type
If the type of value is simple,such as bool,int and std::string,  
it is easy to cast.  
```
const char* json = R"(
    {
        "foo":[1,2,3,4]
    }
)";
std::vector<int> int_list;
JsonSerializer js;
if (!js.Parse(json)) {cout << "parse error" << endl;return 1;}
auto err = js.Unserialize(
    {{"foo",Array<int>(&int_list)}}
);
if(err) return 1;
```

#### Nested Type/Object Type
When you need to cast a nested type,we need a function returns value_list to  
fill in every object of the array.  
```
struct Foo{
    int i;
    std::string s;
};
const char* json = R"(
    {
        "foo_list":[
            {"i":1,"s":"hello world 1"},
            {"i":2,"s":"hello world 2"}
        ]
    }
)";
std::vector<Foo> foo_list;
JsonSerializer js;
if (!js.Parse(json)) {cout << "parse error" << endl;return 1;}
auto err = js.Unserialize(
    {{"foo_list",
     Array<Foo>([](Foo* p) -> Value::ValueList
                                 {
                                     return {
                                         {"i", &p->i},
                                         {"s", &p->s},
                                     };
                                 }}
);
if(err) return 1;
```

## Full Example
```
#include <iostream>
#include <functional>
#include "jsonserializer.h"

using namespace std;
using namespace jsonserializer;

struct Foo
{
    int i;
    std::string s;
};

struct Bar
{
    Foo foo;
    int bar_i;
    std::vector<int> int_list;
    std::vector<Foo> foo_list;
};

const char* json = R"(
{
    "foo":
    {
        "i":1,
        "s":"hello world"
    },
    "bar_i":2,
    "int_list":[1,2,3,4],
    "foo_list":
    [
        {"i":1,"s":"hello world 1"},
        {"i":2,"s":"hello world 2"}
    ]
}
)";

int main(int argc, char** argv)
{
    Bar bar;
    JsonSerializer js;
    if (!js.Parse(json))
        cout << "parse error" << endl;
    auto err = js.Unseralize(
        {{"foo", {
                     {"i", &bar.foo.i},
                     {"s", &bar.foo.s},
                 }},
         {"bar_i", &bar.bar_i},
         {"int_list", Array<int>(&bar.int_list)},
         {"foo_list", Array<Foo>([](Foo* p) -> Value::ValueList
                                 {
                                     return {
                                         {"i", &p->i},
                                         {"s", &p->s},
                                     };
                                 },
                                 &bar.foo_list)}});
    if (err)
    {
        cout << err();
        return 1;
    }
    cout << bar.bar_i << endl;
    cout << bar.foo.i << endl;
    cout << bar.foo.s << endl;
    for (const auto& v : bar.int_list)
        cout << v << endl;
    for (const auto& v : bar.foo_list)
    {
        cout << v.i << endl;
        cout << v.s << endl;
    }
}
```