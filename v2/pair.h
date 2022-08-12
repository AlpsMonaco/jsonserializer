#ifndef __JSON_SERIALIZER_OBJECT_H__
#define __JSON_SERIALIZER_OBJECT_H__

#include "prefix.h"
#include "value.h"

NAMESPACE_JSR_START

class Pair;
using Object = std::vector<Pair>;

class Pair
{
public:
    template <typename T>
    Pair(const std::string& key, T& t)
        : key_(key),
          value_setter_(ValueType<T>::GetSetter(t)),
          value_checker_(ValueType<T>::GetChecker())
    {
    }

    Pair(const std::string& key, const Object& object)
        : key_(key),
          value_setter_(ValueType<Object>::GetSetter(object)),
          value_checker_(ValueType<Object>::GetChecker())
    {
    }

    ~Pair() {}

    const std::string& Key() { return key_; }
    Error operator()(const rapidjson::Value& value)
    {
        if (!value_checker_(value))
        {
            Error error(ErrorCode::kTypeMismatch);
            return error;
        }
        return value_setter_(value);
    }

    const std::string& Key() const { return const_cast<Pair&>(*this).Key(); }
    Error operator()(const rapidjson::Value& value) const { return const_cast<Pair&>(*this)(value); }

protected:
    std::string key_;
    ValueSetter value_setter_;
    ValueChecker value_checker_;
};

NAMESPACE_JSR_END

#endif