#ifndef __JSON_SERIALIZER_JSON_H__
#define __JSON_SERIALIZER_JSON_H__

#include "pair.h"

NAMESPACE_JSR_START

class Json
{
public:
    Json() : d_() {}
    ~Json() {}

    Error Parse(const std::string& json_string)
    {
        d_.Parse(json_string.c_str());
        if (d_.HasParseError())
            return Error(ErrorCode::kParseJsonError);
        return Error();
    }

    Error Deserialize(const Object& object)
    {
        return Parser::Parse(d_, object);
    }

protected:
    rapidjson::Document d_;
};

NAMESPACE_JSR_END

#endif