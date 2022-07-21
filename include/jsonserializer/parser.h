#ifndef __JSON_SERIALIZER_PARSER_H__
#define __JSON_SERIALIZER_PARSER_H__

#include "prefix.h"
#include "rapidjson/document.h"
#include "errors.h"

JSON_SERIALIZER_NAMESPACE_START

class Parser
{
public:
    template <typename ExternValueType>
    static Error Parse(const rapidjson::Value& object, const ExternValueType& value)
    {
        if (!object.HasMember(value.Key())) { return Error(Error::ErrorCode::kKeyNotExist); }
        switch (value.Type())
        {
        case ValueType::kInt:
            if (!object[value.Key()].IsInt()) { return Error(Error::ErrorCode::kNotAInt); }
            break;
        case ValueType::kString:
            if (!object[value.Key()].IsString()) { return Error(Error::ErrorCode::kNotAString); }
            break;
        case ValueType::kBool:
            if (!object[value.Key()].IsBool()) { return Error(Error::ErrorCode::kNotABool); }
            break;
        case ValueType::kObject:
            if (!object[value.Key()].IsObject()) { return Error(Error::ErrorCode::kNotAObject); }
            break;
        }
        return value(object[value.Key()]);
    }
};

JSON_SERIALIZER_NAMESPACE_END

#endif