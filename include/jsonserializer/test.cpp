#include <iostream>
#include <functional>
#include "jsonserializer.h"

using namespace std;

struct Foo2
{
    int bar_1;
    int bar_2;
};

struct Foo
{
    int bar_1;
    const char* bar_2;
    bool bar_3;
    Foo2 foo2;
};

struct Bar
{
    Bar(int i)
    {
        f_ = [&]() -> void
        {
            cout << i << endl;
        };
    }
    ~Bar() { cout << "~Bar" << endl; }

    std::function<void()> GetCaller()
    {
        return f_;
    }

    std::function<void()> f_;
};

int main(int argc, char** argv)
{
    Foo foo;
    jsonserializer::JsonSerializer js;
    auto result = js.Parse(R"({"foo":"bar","bar":2,"b":true,"object":{"asd":"bar"}})", {{"foo", &foo.bar_2},
                                                                                        {"bar", &foo.bar_1},
                                                                                        {"b", &foo.bar_3},
                                                                                        {"object", &foo.foo2}});
    if (result != jsonserializer::ParseResult::kSuccess)
    {
        cout << js.GetError() << endl;
        return 1;
    }
}