#ifndef __JSON_SERIALIZER_ARRAY_H__
#define __JSON_SERIALIZER_ARRAY_H__

#include "value.h"

NAMESPACE_JSR_START

class Array
{
public:
    Array(const rapidjson::Value& array_value)
        : array_value_(&array_value),
          size_(array_value.IsArray() ? array_value_->Size() : 0)
    {
    }

    Array()
        : array_value_(&static_cast<const rapidjson::Value&>(EmptyArray::Get())),
          size_(0)
    {
    }

    Array(const Array& array)
        : array_value_(array.array_value_),
          size_(array.size_)
    {
    }

    Array& operator=(const Array& rhs)
    {
        array_value_ = rhs.array_value_;
        size_ = rhs.size_;
        return *this;
    }

    ~Array()
    {
    }

    size_t Size()
    {
        return size_;
    }

    Value operator[](size_t index)
    {
        return At(index);
    }

    Value At(size_t index)
    {
        if (index > size_)
            return Value(NullValue::Get());
        return Value((*array_value_)[index]);
    }

protected:
    const rapidjson::Value* array_value_;
    size_t size_;
};

NAMESPACE_JSR_END

#endif