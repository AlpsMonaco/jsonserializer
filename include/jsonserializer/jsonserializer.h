#ifndef __JSON_SERIALIZER_H__
#define __JSON_SERIALIZER_H__

#include "jsonvalue.h"
#include <string>

JSON_SERIALIZER_NAMESPACE_START

class JsonSerializer
{
public:
    bool Seralize();
    inline bool Parse(const char* json)
    {
        d_.Parse(json);
        return !d_.HasParseError();
    }
    inline const char* GetError() { return err_.c_str(); }
    bool Unseralize(const Value::ValueList& value_list)
    {
        ParseResult result;
        for (const auto& v : value_list)
        {
            result = Parser::Parse<Value, std::string>(d_, v, err_);
            if (result != ParseResult::kSuccess)
            {
                err_ = v.GetError();
                return false;
            }
        }
        return true;
    }

protected:
    ::rapidjson::Document d_;
    ::std::string err_;
};

JSON_SERIALIZER_NAMESPACE_END

#endif