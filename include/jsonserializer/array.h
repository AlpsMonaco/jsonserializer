#ifndef __JSON_SERIALIZER_ARRAY_H__
#define __JSON_SERIALIZER_ARRAY_H__

#include "value.h"

JSON_SERIALIZER_NAMESPACE_START

template <typename T>
struct IsPlainType
{
    constexpr static bool value = (std::is_arithmetic<T>::value ||
                                   std::is_same<std::string, T>::value);
};

template <typename T, typename = void>
struct ArrayItem
{
    constexpr static ValueType type_value = ValueType::kObject;
    static bool CheckValue(const rapidjson::Value& value) { return value.IsObject(); }
    constexpr static Error::ErrorCode type_error = Error::ErrorCode::kNotAObject;
};

template <typename T>
struct ArrayItem<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
    constexpr static ValueType type_value = ValueType::kInt;
    static bool CheckValue(const rapidjson::Value& value) { return value.IsInt(); }
    static T GetValue(const rapidjson::Value& value) { return value.GetInt(); }
    constexpr static Error::ErrorCode type_error = Error::ErrorCode::kNotAInt;
};

template <>
struct ArrayItem<std::string, void>
{
    constexpr static ValueType type_value = ValueType::kString;
    static bool CheckValue(const rapidjson::Value& value) { return value.IsString(); }
    static std::string GetValue(const rapidjson::Value& value) { return std::string(value.GetString()); }
    constexpr static Error::ErrorCode type_error = Error::ErrorCode::kNotAString;
};

template <>
struct ArrayItem<bool, void>
{
    constexpr static ValueType type_value = ValueType::kBool;
    static bool CheckValue(const rapidjson::Value& value) { return value.IsBool(); }
    static bool GetValue(const rapidjson::Value& value) { return value.GetBool(); }
    constexpr static Error::ErrorCode type_error = Error::ErrorCode::kNotABool;
};

template <typename DataType, typename = void>
class Array
{
public:
    using ArrayItemType = ArrayItem<DataType>;
    using DataList      = std::vector<DataType>;
    using Handler       = std::function<Value::ValueList(DataType*)>;

    Array(const Handler& handler, DataList* list_ptr)
        : handler_(handler),
          list_ptr_(list_ptr)
    {
    }
    ~Array() {}

    Error operator()(const rapidjson::Value& arr) const
    {
        for (rapidjson::SizeType i = 0; i < arr.Size(); i++)
        {
            const rapidjson::Value& value = arr[i];
            if(!value.IsObject())
                 return Error(Error::ErrorCode::kNotAObject);
            DataType data;
            auto value_list = handler_(&data);
            DataType* data_ptr = &data;
            Error error;
            for (const auto& v : value_list)
            {
                error = Parser::Parse<Value>(value, v);
                if (error)
                    return Error(error, v.Key());
            }
            list_ptr_->emplace_back(std::move(data));
        }
        return Error();
    }

protected:
    Handler handler_;
    DataList* list_ptr_;
};

template <typename DataType>
class Array<DataType, typename std::enable_if<IsPlainType<DataType>::value>::type>
{
public:
    using ArrayItemType = ArrayItem<DataType>;
    using DataList      = std::vector<DataType>;

    Array(DataList* list_ptr) : list_ptr_(list_ptr) {}
    ~Array() {}

    Error operator()(const rapidjson::Value& value) const
    {
        {
            for (rapidjson::SizeType i = 0; i < value.Size(); i++)
                if (!ArrayItemType::CheckValue(value[i]))
                    return Error(ArrayItemType::type_error);
                else
                    list_ptr_->emplace_back(ArrayItemType::GetValue(value[i]));
        }
        return Error();
    }

protected:
    DataList* list_ptr_;
};

JSON_SERIALIZER_NAMESPACE_END
#endif