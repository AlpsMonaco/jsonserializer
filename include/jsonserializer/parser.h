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
    template <typename ExternValueType, typename ErrorWriter>
    static ParseResult Parse(const rapidjson::Value& object, const ExternValueType& value, ErrorWriter& error_writer)
    {
        if (!object.HasMember(value.Key()))
        {
            error_writer = value.Key();
            error_writer += " not exist";
            return ParseResult::kKeyNotExist;
        }
        switch (value.Type())
        {
        case ValueType::kInt:
            if (!object[value.Key()].IsInt())
            {
                error_writer = value.Key();
                error_writer += " not a int";
                return ParseResult::kTypeNotMatch;
            }
            break;
        case ValueType::kString:
            if (!object[value.Key()].IsString())
            {
                error_writer = value.Key();
                error_writer += " not a string";
                return ParseResult::kTypeNotMatch;
            }
            break;
        case ValueType::kBool:
            if (!object[value.Key()].IsBool())
            {
                error_writer = value.Key();
                error_writer += " not a bool";
                return ParseResult::kTypeNotMatch;
            }
            break;
        case ValueType::kObject:
            if (!object[value.Key()].IsObject())
            {
                error_writer = value.Key();
                error_writer += " not a object";
                return ParseResult::kTypeNotMatch;
            }
            break;
        }
        return value(object[value.Key()]);
    }
};

JSON_SERIALIZER_NAMESPACE_END

#endif