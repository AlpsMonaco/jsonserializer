#ifndef __JSON_SERIALIZER_VALUE_H__
#define __JSON_SERIALIZER_VALUE_H__

#include <type_traits>
#include "prefix.h"
#include "error.h"
#include "parser.h"

NAMESPACE_JSR_START

template <typename T, typename = void>
struct IsBasicType
{
    static constexpr bool value = false;
};

template <typename T>
struct IsBasicType<T,
                   typename std::enable_if<std::is_arithmetic<T>::value ||
                                           std::is_same<std::string, T>::value ||
                                           std::is_same<std::string_view, T>::value>::type>
{
    static constexpr bool value = true;
};

template <typename T, typename = void>
struct BasicType
{
    static auto GetValue(const rapidjson::Value& value)
    {
        return value.GetString();
    }

    static bool CheckType(const rapidjson::Value& value)
    {
        return value.IsString();
    }
};

template <>
struct BasicType<bool>
{
    static auto GetValue(const rapidjson::Value& value)
    {
        return value.GetBool();
    }

    static bool CheckType(const rapidjson::Value& value)
    {
        return value.IsBool();
    }
};

template <>
struct BasicType<std::int64_t>
{
    static auto GetValue(const rapidjson::Value& value)
    {
        return value.GetInt64();
    }

    static bool CheckType(const rapidjson::Value& value)
    {
        return value.IsInt64();
    }
};

template <>
struct BasicType<std::uint64_t>
{
    static auto GetValue(const rapidjson::Value& value)
    {
        return value.GetUint64();
    }

    static bool CheckType(const rapidjson::Value& value)
    {
        return value.IsUint64();
    }
};

template <>
struct BasicType<float>
{
    static auto GetValue(const rapidjson::Value& value)
    {
        return value.GetFloat();
    }

    static bool CheckType(const rapidjson::Value& value)
    {
        return value.IsFloat();
    }
};

template <typename T>
struct BasicType<T, typename std::enable_if<
                        std::is_floating_point<T>::value>::
                        type>
{
    static auto GetValue(const rapidjson::Value& value)
    {
        return value.GetDouble();
    }

    static bool CheckType(const rapidjson::Value& value)
    {
        return value.IsDouble();
    }
};

template <typename T>
struct BasicType<T, typename std::enable_if<
                        std::is_integral<T>::value &&
                        std::is_unsigned<T>::value>::
                        type>
{
    static auto GetValue(const rapidjson::Value& value)
    {
        return value.GetUint();
    }

    static bool CheckType(const rapidjson::Value& value)
    {
        return value.IsUint();
    }
};

template <typename T>
struct BasicType<T, typename std::enable_if<
                        std::is_integral<T>::value &&
                        !std::is_unsigned<T>::value>::
                        type>
{
    static auto GetValue(const rapidjson::Value& value)
    {
        return value.GetInt();
    }

    static bool CheckType(const rapidjson::Value& value)
    {
        return value.IsInt();
    }
};

struct SelectByVectorMethod
{
    template <typename T>
    static constexpr auto IsVector() -> decltype(std::declval<T>().begin(),
                                                 std::declval<T>().end(),
                                                 std::declval<T>().emplace_back(std::declval<T::value_type>()),
                                                 bool())
    {
        return true;
    }

    template <typename T>
    static constexpr auto IsVector(...) -> bool
    {
        return false;
    }

    template <typename T>
    struct IsVectorType
    {
        static constexpr bool value = IsVector<T>();
    };
};

template <typename T, typename = void>
struct IsVectorType
{
    static constexpr bool value = false;
};

template <typename T>
struct IsVectorType<T,
                    typename std::enable_if<std::is_same<T, std::vector<typename T::value_type>>::value>::type>
{
    static constexpr bool value = true;
};

using ValueSetter = std::function<Error(const rapidjson::Value&)>;
using ValueChecker = std::function<bool(const rapidjson::Value&)>;

template <typename T, typename = void>
class ValueType
{
public:
    static ValueSetter GetSetter(const T& t)
    {
        return [&](const rapidjson::Value& value) -> Error
        {
            return Parser::Parse(value, t);
        };
    }

    static ValueChecker GetChecker()
    {
        return [](const rapidjson::Value& value) -> bool
        {
            return value.IsObject();
        };
    }
};

template <typename T>
constexpr auto IsAbstractArrayType(...) -> decltype(bool())
{
    return false;
}

template <typename T>
constexpr auto IsAbstractArrayType(T* t) -> decltype(std::declval<T>().Size(),
                                                     std::declval<T>().At(std::size_t()),
                                                     std::declval<T>()[std::size_t()],
                                                     bool())
{
    return true;
}

template <typename T>
struct IsAbstractArray
{
    static constexpr bool value = IsAbstractArrayType<T>(nullptr);
};

template <typename T>
class ValueType<T, typename std::enable_if<IsBasicType<T>::value>::type>
{
public:
    static ValueSetter GetSetter(T& t)
    {
        return [&](const rapidjson::Value& value) -> Error
        {
            t = BasicType<T>::GetValue(value);
            return Error();
        };
    }

    static ValueChecker GetChecker()
    {
        return [](const rapidjson::Value& value) -> bool
        {
            return BasicType<T>::CheckType(value);
        };
    }
};

template <typename T>
class ValueType<T, typename std::enable_if<IsVectorType<T>::value &&
                                           IsBasicType<typename T::value_type>::value>::type>
{
public:
    static ValueSetter GetSetter(T& t)
    {
        return [&](const rapidjson::Value& array_value) -> Error
        {
            for (rapidjson::SizeType i = 0; i < array_value.Size(); i++)
            {
                const auto& value = array_value[i];
                if (!BasicType<typename T::value_type>::CheckType(value))
                {
                    Error error(ErrorCode::kTypeMismatchInArray);
                    error += std::to_string(i);
                    return error;
                }
                else
                    t.emplace_back(BasicType<typename T::value_type>::GetValue(value));
            }
            return Error();
        };
    }

    static ValueChecker GetChecker()
    {
        return [](const rapidjson::Value& value) -> bool
        {
            return value.IsArray();
        };
    }
};

template <typename T>
class ValueType<T,
                typename std::enable_if<IsAbstractArray<T>::value>::type>
{
public:
    static ValueSetter GetSetter(T& t)
    {
        return [&](const rapidjson::Value& array_value) -> Error
        {
            t = T(array_value);
            return Error();
        };
    }

    static ValueChecker GetChecker()
    {
        return [](const rapidjson::Value& value) -> bool
        {
            return value.IsArray();
        };
    }
};

template <>
class ValueType<const char**, void>
{
public:
    static ValueSetter GetSetter(const char** p)
    {
        return [=](const rapidjson::Value& value) -> Error
        {
            *p = value.GetString();
            return Error();
        };
    }

    static ValueChecker GetChecker()
    {
        return [](const rapidjson::Value& value) -> bool
        {
            return value.IsString();
        };
    }
};

NAMESPACE_JSR_END

#endif