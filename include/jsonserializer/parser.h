#ifndef __JSON_SERIALIZER_PARSER_H__
#define __JSON_SERIALIZER_PARSER_H__

#include "prefix.h"

JSON_SERIALIZER_NAMESPACE_START

enum class ParseResult : int
{
    kSuccess,
    kKeyNotExist,
    kTypeNotMatch,
};

class Parser
{
public:
    template <typename ExternValueType>
    static ParseResult Parse(const rapidjson::Value& object, const ExternValueType& value)
    {
        if (!object.HasMember(value.Key())) return ParseResult::kKeyNotExist;
        switch (value.Type())
        {
        case ValueType::kInt:
            if (!object[value.Key()].IsInt()) { return ParseResult::kTypeNotMatch; }
            break;
        case ValueType::kString:
            if (!object[value.Key()].IsString()) { return ParseResult::kTypeNotMatch; }
            break;
        case ValueType::kBool:
            if (!object[value.Key()].IsBool()) { return ParseResult::kTypeNotMatch; }
            break;
        case ValueType::kObject:
            if (!object[value.Key()].IsObject()) { return ParseResult::kTypeNotMatch; }
            break;
        }
        return value(object[value.Key()]);
    }
};

JSON_SERIALIZER_NAMESPACE_END

#endif