#ifndef __JSON_SERIALIZER_JSON_H__
#define __JSON_SERIALIZER_JSON_H__

#include "pair.h"
#include "array.h"

NAMESPACE_JSR_START

using Document = Value;

class Json
{
public:
    Json() : d_() {}
    ~Json() {}

    Error Parse(const std::string& json_string)
    {
        return Parse(json_string.c_str());
    }

    Error Parse(const char* json_string)
    {
        d_.Parse(json_string);
        if (d_.HasParseError())
            return Error(ErrorCode::kParseJsonError);
        return Error();
    }

    Error Unmarshal(const Object& object)
    {
        return Parser::Parse(d_, object);
    }

    Document Deserialize()
    {
        return Document(d_);
    }

protected:
    rapidjson::Document d_;
};

NAMESPACE_JSR_END

#endif