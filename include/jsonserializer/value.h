#ifndef __JSON_SERIALIZER_VALUE_H__
#define __JSON_SERIALIZER_VALUE_H__

#include "prefix.h"
#include "valuetype.h"
#include "pair.h"

NAMESPACE_JSR_START

class NullValue
{
public:
    NullValue() { value_.SetNull(); }
    ~NullValue() {}

    operator const rapidjson::Value&()
    {
        return value_;
    }

    operator const rapidjson::Value&() const
    {
        return value_;
    }

    operator const rapidjson::Value*() const
    {
        return &value_;
    }

    static const NullValue& Get()
    {
        static NullValue null_value;
        return null_value;
    }

protected:
    rapidjson::Value value_;
};

class EmptyObject
{
public:
    EmptyObject() { value_.SetObject(); }
    ~EmptyObject() {}

    operator const rapidjson::Value&()
    {
        return value_;
    }

    operator const rapidjson::Value&() const
    {
        return value_;
    }

    static const EmptyObject& Get()
    {
        static EmptyObject empty_object;
        return empty_object;
    }

    static auto Begin()
    {
        return Get().value_.MemberBegin();
    }

    static auto End()
    {
        return Get().value_.MemberEnd();
    }

protected:
    rapidjson::Value value_;
};

class EmptyArray
{
public:
    EmptyArray() { value_.SetArray(); }
    ~EmptyArray() {}

    operator const rapidjson::Value&()
    {
        return value_;
    }

    operator const rapidjson::Value&() const
    {
        return value_;
    }

    static const EmptyArray& Get()
    {
        static EmptyArray empty_array;
        return empty_array;
    }

protected:
    rapidjson::Value value_;
};

class Value
{
public:
    Value(const rapidjson::Value& val)
        : val_(&val)
    {
    }

    Value()
        : val_(NullValue::Get())
    {
    }

    template <typename T>
    operator T()
    {
        if (!ValueType<T>::GetChecker()(*val_)) return T();
        T t;
        ValueType<T>::GetSetter(t)(*val_);
        return t;
    }

    operator const char*()
    {
        if (!val_->IsString()) return "";
        return val_->GetString();
    }

    Value operator[](const char* key)
    {
        if (!(*val_).IsObject()) return Value(NullValue::Get());
        return (*val_)[key];
    }

    auto Begin()
    {
        if (!val_->IsObject()) return EmptyObject::Begin();
        return val_->MemberBegin();
    }

    auto End()
    {
        if (!val_->IsObject()) return EmptyObject::End();
        return val_->MemberEnd();
    }

    bool IsNull()
    {
        return !val_->IsNull();
    }

    operator bool()
    {
        return IsNull();
    }

    Error Unmarshal(const Object& object)
    {
        return Parser::Parse(*val_, object);
    }

protected:
    const rapidjson::Value* val_;
};

NAMESPACE_JSR_END

#endif