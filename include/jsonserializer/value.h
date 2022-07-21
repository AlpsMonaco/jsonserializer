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

    template <typename T>
    struct IsPlainType
    {
        constexpr static bool value = (std::is_arithmetic<T>::value || std::is_same<std::string, T>::value);
    };

    template <typename T, typename = void>
    struct TypeId
    {
        constexpr static ValueType value = ValueType::kObject;
        static bool CheckType(const rapidjson::Value& value) { return value.IsObject(); }
    };

    template <typename T>
    struct TypeId<T, std::enable_if<std::is_integral<T>::value>::type>
    {
        constexpr static ValueType value = ValueType::kInt;
        static bool CheckType(const rapidjson::Value& value) { return value.IsInt(); }
    };

    template <typename T>
    struct TypeId<T, std::enable_if<std::is_same<std::string, T>::value>::type>
    {
        constexpr static ValueType value = ValueType::kString;
        static bool CheckType(const rapidjson::Value& value) { return value.IsString(); }
    };

    template <>
    struct TypeId<bool, void>
    {
        constexpr static ValueType value = ValueType::kBool;
        static bool CheckType(const rapidjson::Value& value) { return value.IsBool(); }
    };

    using ObjectType = void;
    template <typename T, typename = ObjectType>
    class Array
    {
    public:
        using ListType = std::vector<T>;
        Array() {}
        ~Array() {}

        static constexpr ValueType type_id = TypeId<T>;
        inline std::vector<T>& operator std::vector<T>() { return list_; }
        inline std::vector<T>& Get() { return list_; }

    protected:
        std::vector<T> list_;
    };

    template <typename T>
    class Array<T, std::enable_if<IsPlainType<T>::value>::type>
    {
    public:
        using ListType = std::vector<T>;
        Array() {}
        ~Array() {}

        static constexpr ValueType type_id = TypeId<T>;
        inline std::vector<T>& operator std::vector<T>() { return list_; }
        inline std::vector<T>& Get() { return list_; }
        Error operator()(const rapidjson::Value& value)
        {
        }

    protected:
        std::vector<T> list_;
    };

    Value(const char* key, const ValueList& value_list)
        : key_(key),
          value_type_(ValueType::kObject),
          value_list_(std::make_unique<ValueList>(value_list)),
          reactor_([&](const rapidjson::Value& value) -> Error
                   {
                       Error errors;
                       for (const auto& v : *(value_list_))
                       {
                           errors = Parser::Parse<Value>(value, v);
                           if (errors)
                           {
                               errors = Error(errors, v.Key());
                               break;
                           }
                       }
                       return errors;
                   })
    {
    }

    Value(const char* key, ValueList&& value_list)
        : key_(key),
          value_type_(ValueType::kObject),
          value_list_(std::make_unique<ValueList>(std::move(value_list))),
          reactor_([&](const rapidjson::Value& value) -> Error
                   {
                       Error errors;
                       for (const auto& v : *(value_list_))
                       {
                           errors = Parser::Parse<Value>(value, v);
                           if (errors)
                           {
                               errors = Error(std::move(errors), v.Key());
                               break;
                           }
                       }
                       return errors;
                   })
    {
    }

    Value(const char* key, int* p)
        : key_(key),
          value_type_(ValueType::kInt),
          value_list_(),
          reactor_([&](const rapidjson::Value& value) -> Error
                   {
                       *pointer_field_.ptr_int = value.GetInt();
                       return Error();
                   })
    {
        pointer_field_.ptr_int = p;
    }
    Value(const char* key, bool* p)
        : key_(key),
          value_type_(ValueType::kBool), value_list_(),
          reactor_([&](const rapidjson::Value& value) -> Error
                   {
                       *pointer_field_.ptr_bool = value.GetBool();
                       return Error();
                   })
    {
        pointer_field_.ptr_bool = p;
    }
    Value(const char* key, std::string* p)
        : key_(key),
          value_type_(ValueType::kString), value_list_(),
          reactor_([&](const rapidjson::Value& value) -> Error
                   {
                       *pointer_field_.ptr_string = value.GetString();
                       return Error();
                   })
    {
        pointer_field_.ptr_string = p;
    }

    Value(const Value& value)
        : key_(value.key_),
          value_type_(value.value_type_),
          pointer_field_(value.pointer_field_),
          reactor_(value.reactor_),
          value_list_()
    {
        if (value_type_ == ValueType::kObject)
            value_list_ = std::make_unique<ValueList>(*value.value_list_);
    }
    Value(Value&& value)
        : key_(value.key_),
          value_type_(value.value_type_),
          pointer_field_(value.pointer_field_),
          reactor_(value.reactor_),
          value_list_()
    {
        if (value_type_ == ValueType::kObject)
            value_list_ = std::move(value.value_list_);
    }
    ~Value() {}

    inline Error operator()(const rapidjson::Value& value) { return reactor_(value); }
    inline Error operator()(const rapidjson::Value& value) const { return reactor_(value); }
    inline const char* Key() { return key_; }
    inline const char* Key() const { return key_; }
    inline ValueType Type() { return value_type_; }
    inline ValueType Type() const { return value_type_; }

protected:
    const char* key_;
    ValueType value_type_;
    std::function<Error(const rapidjson::Value& value)> reactor_;
    std::unique_ptr<ValueList> value_list_;

    union PointerField
    {
        int* ptr_int;
        bool* ptr_bool;
        std::string* ptr_string;
    } pointer_field_;
};

JSON_SERIALIZER_NAMESPACE_END

#endif