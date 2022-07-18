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
    Errors Unseralize(const Value::ValueList& value_list)
    {
        Errors errors;
        for (const auto& v : value_list)
        {
            errors = Parser::Parse<Value>(d_, v);
            if (errors)
            {
                errors = Errors(std::move(errors), v.Key());
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