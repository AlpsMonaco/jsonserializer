#include <iostream>
#include <functional>
#include "jsonserializer.h"

using namespace std;
using namespace jsonserializer;

struct Foo
{
    int I;
    const char* S;
    bool B;
};

struct Bar
{
    int I;
    const char* S;
    bool B;
    Foo f;
};

int main(int argc, char** argv)
{
    const char* json = R"(
        {
            "int":1,
            "string":"string",
            "bool":true,
            "object":
            {
                "int":2,
                "string":"string",
                "object":
                {
                    "int":3,
                    "string":"string"
                }
            }
        }
    )";
    int IValue;
    Bar bar;
    JsonSerializer js;
    if (!js.Parse(json))
        return 1;
    bool b = js.Unseralize({{"int2", &bar.I},
                            {"string", &bar.S},
                            {"bool", &bar.B},
                            {"object", {{{"string", &bar.f.S}, {"int", &bar.f.I}, {"object", {{"int", &IValue}}}}}}});

    if (b)
    {
        cout << bar.I << endl;
        cout << bar.S << endl;
        cout << bar.B << endl;
        cout << bar.f.I << endl;
        cout << bar.f.S << endl;
    }
    else
    {
        cout << js.GetError();
    }
}