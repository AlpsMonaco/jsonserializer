#ifndef __JSON_SERIALIZER_H__
#define __JSON_SERIALIZER_H__

#include "array.h"
#include <string>

JSON_SERIALIZER_NAMESPACE_START

class JsonSerializer
{
public:
    bool Seralize()
    {
        THROW_NOT_IMPLEMENT;
        return true;
    }

    inline bool Parse(const char* json)
    {
        d_.Parse(json);
        return !d_.HasParseError();
    }

    Error Unseralize(const Value::ValueList& value_list)
    {
        Error errors;
        for (const auto& v : value_list)
        {
            errors = Parser::Parse<Value>(d_, v);
            if (errors)
            {
                errors = Error(std::move(errors), v.Key());
                break;
            }
        }
        return errors;
    }

protected:
    ::rapidjson::Document d_;
};

JSON_SERIALIZER_NAMESPACE_END

#endif