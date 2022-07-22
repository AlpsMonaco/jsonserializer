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
                                         /* {"s", &p->s}, */
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