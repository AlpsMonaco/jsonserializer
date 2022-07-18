#ifndef __JSON_SERIALIZER_PARSER_H__
#define __JSON_SERIALIZER_PARSER_H__

#include "prefix.h"
#include "errors.h"

JSON_SERIALIZER_NAMESPACE_START

class Parser
{
public:
    template <typename ExternValueType>
    static Errors Parse(const rapidjson::Value& object, const ExternValueType& value)
    {
        if (!object.HasMember(value.Key())) { return Errors(std::string(value.Key()) + " not exist"); }
        switch (value.Type())
        {
        case ValueType::kInt:
            if (!object[value.Key()].IsInt()) { return Errors(std::string(value.Key()) + " is not a int"); }
            break;
        case ValueType::kString:
            if (!object[value.Key()].IsString()) { return Errors(std::string(value.Key()) + " is not a string"); }
            break;
        case ValueType::kBool:
            if (!object[value.Key()].IsBool()) { return Errors(std::string(value.Key()) + " is not a bool"); }
            break;
        case ValueType::kObject:
            if (!object[value.Key()].IsObject()) { return Errors(std::string(value.Key()) + " is not a object"); }
            break;
        }
        return value(object[value.Key()]);
    }
};

JSON_SERIALIZER_NAMESPACE_END

#endif