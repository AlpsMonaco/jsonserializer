#ifndef __JSON_SERIALIZER_OBJECT_H__
#define __JSON_SERIALIZER_OBJECT_H__

#include "parser.h"
#include <functional>
#include <vector>
#include <string>

JSON_SERIALIZER_NAMESPACE_START

class Value
{
public:
    using ValueList = std::vector<Value>;

    template <typename ArrayType>
    Value(const char* key, const ArrayType& array)
        : key_(key),
          value_type_(ValueType::kArray),
          value_list_(),
          array_setter_([array](const rapidjson::Value& value) -> Error
                        {
                            return array(value);
                        })
    {
    }

    template <typename ArrayType>
    Value(const char* key, ArrayType&& array)
        : key_(key),
          value_type_(ValueType::kArray),
          value_list_(),
          array_setter_([array{std::move(array)}](const rapidjson::Value& value) -> Error
                        {
                            return array(value);
                        })
    {
    }

    Value(const char* key, const ValueList& value_list)
        : key_(key),
          value_type_(ValueType::kObject),
          value_list_(std::make_unique<ValueList>(value_list)) {}

    Value(const char* key, ValueList&& value_list)
        : key_(key),
          value_type_(ValueType::kObject),
          value_list_(std::make_unique<ValueList>(std::move(value_list))) {}

    Value(const char* key, int* p)
        : key_(key),
          value_type_(ValueType::kInt),
          value_list_() { pointer_field_.ptr_int = p; }

    Value(const char* key, int64_t* p)
        : key_(key),
          value_type_(ValueType::kInt64),
          value_list_() { pointer_field_.ptr_int64 = p; }

    Value(const char* key, bool* p)
        : key_(key),
          value_type_(ValueType::kBool),
          value_list_() { pointer_field_.ptr_bool = p; }

    Value(const char* key, std::string* p)
        : key_(key),
          value_type_(ValueType::kString),
          value_list_() { pointer_field_.ptr_string = p; }

    Value(const Value& value)
        : key_(value.key_),
          value_type_(value.value_type_),
          pointer_field_(value.pointer_field_),
          array_setter_(value.array_setter_),
          value_list_()
    {
        if (value_type_ == ValueType::kObject)
            value_list_ = std::make_unique<ValueList>(*value.value_list_);
    }

    Value(Value&& value)
        : key_(value.key_),
          value_type_(value.value_type_),
          pointer_field_(value.pointer_field_),
          array_setter_(value.array_setter_),
          value_list_()
    {
        if (value_type_ == ValueType::kObject)
            value_list_ = std::move(value.value_list_);
    }
    ~Value() {}

    inline const char* Key() { return key_; }
    inline const char* Key() const { return key_; }
    inline ValueType Type() { return value_type_; }
    inline ValueType Type() const { return value_type_; }

    Error SetInt(const rapidjson::Value& value)
    {
        *pointer_field_.ptr_int = value.GetInt();
        return Error();
    }

    Error SetInt64(const rapidjson::Value& value)
    {
        *pointer_field_.ptr_int64 = value.GetInt64();
        return Error();
    }

    Error SetString(const rapidjson::Value& value)
    {
        *pointer_field_.ptr_string = value.GetString();
        return Error();
    }

    Error SetBool(const rapidjson::Value& value)
    {
        *pointer_field_.ptr_bool = value.GetBool();
        return Error();
    }

    Error SetObject(const rapidjson::Value& value)
    {
        Error error;
        for (const auto& v : *(value_list_))
        {
            error = Parser::Parse<Value>(value, v);
            if (error)
            {
                error = Error(error, v.Key());
                break;
            }
        }
        return error;
    }

    Error SetArray(const rapidjson::Value& value) { return array_setter_(value); }
    Error SetInt(const rapidjson::Value& value) const { return const_cast<Value&>(*this).SetInt(value); }
    Error SetString(const rapidjson::Value& value) const { return const_cast<Value&>(*this).SetString(value); }
    Error SetBool(const rapidjson::Value& value) const { return const_cast<Value&>(*this).SetBool(value); }
    Error SetObject(const rapidjson::Value& value) const { return const_cast<Value&>(*this).SetObject(value); }
    Error SetArray(const rapidjson::Value& value) const { return const_cast<Value&>(*this).SetArray(value); }

protected:
    const char* key_;
    ValueType value_type_;
    std::function<Error(const rapidjson::Value& value)> array_setter_;
    std::unique_ptr<ValueList> value_list_;
    union PointerField
    {
        int64_t* ptr_int64;
        int* ptr_int;
        bool* ptr_bool;
        std::string* ptr_string;
    } pointer_field_;
};

JSON_SERIALIZER_NAMESPACE_END

#endif