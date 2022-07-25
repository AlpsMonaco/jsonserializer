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
        const rapidjson::Value& temp_object = object[value.Key()];
        switch (value.Type())
        {
        case ValueType::kInt:
            if (!temp_object.IsInt())
                return Error(Error::ErrorCode::kNotAInt);
            return value.SetInt(temp_object);
        case ValueType::kString:
            if (!temp_object.IsString())
                return Error(Error::ErrorCode::kNotAString);
            return value.SetString(temp_object);
        case ValueType::kBool:
            if (!temp_object.IsBool())
                return Error(Error::ErrorCode::kNotABool);
            return value.SetBool(temp_object);
        case ValueType::kObject:
            if (!temp_object.IsObject())
                return Error(Error::ErrorCode::kNotAObject);
            return value.SetObject(temp_object);
        }
        if (!temp_object.IsArray())
            return Error(Error::ErrorCode::kNotAnArray);
        return value.SetArray(temp_object);
    }
};

JSON_SERIALIZER_NAMESPACE_END

#endif