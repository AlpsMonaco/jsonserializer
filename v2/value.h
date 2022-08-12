#ifndef __JSON_SERIALIZER_DATA_H__
#define __JSON_SERIALIZER_DATA_H__

#include "prefix.h"
#include "valuetype.h"

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

    static NullValue& Get()
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

    static EmptyObject& Get()
    {
        static EmptyObject empty_object;
        return empty_object;
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

    static EmptyArray& Get()
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
        : val_(val) {}

    template <typename T>
    operator T()
    {
        if (!ValueType<T>::GetChecker()(val_)) return T();
        T t;
        ValueType<T>::GetSetter(t)(val_);
        return t;
    }

    Value operator[](const char* key)
    {
        if (!val_.IsObject()) return Value(NullValue::Get());
        return val_[key];
    }

protected:
    const rapidjson::Value& val_;
};

NAMESPACE_JSR_END

#endif