#ifndef __JSON_SERIALIZER__
#define __JSON_SERIALIZER__

#include "../rapidjson/include/rapidjson/document.h"
#include <vector>
#include <string>
#include <functional>

#define JSON_SERIALIZER_START \
    namespace jsonserializer  \
    {

#define JSON_SERIALIZER_END }
#define THROW_NOT_IMPLEMENT throw std::exception("not implement yet");

JSON_SERIALIZER_START

class Binder
{
public:
    enum class Type : int
    {
        kInt,
        kBool,
        kString,
        kArray,
        kObject,
        kNull,
    };

    Binder(const char* key, int* to) : key_(key), type_id_(Type::kInt)
    {
        pointer_field_.p_int = to;
        setter_              = [&](const rapidjson::Value& val) -> void
        {
            *pointer_field_.p_int = val.GetInt();
        };
    }
    Binder(const char* key, bool* to) : key_(key), type_id_(Type::kInt)
    {
        pointer_field_.p_bool = to;
        setter_               = [&](const rapidjson::Value& val) -> void
        {
            *pointer_field_.p_bool = val.GetBool();
        };
    }
    Binder(const char* key, const char** to) : key_(key), type_id_(Type::kString)
    {
        pointer_field_.p_str = to;
        setter_              = [&](const rapidjson::Value& val) -> void
        {
            *pointer_field_.p_str = val.GetString();
        };
    }

    ~Binder() {}

    const char* Key() { return key_; }
    const char* Key() const { return key_; }

    Type TypeId() { return type_id_; }
    Type TypeId() const { return type_id_; }

    void operator()(const rapidjson::Value& val) { setter_(val); }
    void operator()(const rapidjson::Value& val) const { setter_(val); }

protected:
    const char* key_;
    union PointerField
    {
        int* p_int;
        bool* p_bool;
        const char** p_str;
    } pointer_field_;
    std::function<void(const rapidjson::Value&)> setter_;
    Type type_id_;
};

class JsonSerializer
{
public:
    enum class ParseResult : int
    {
        kSuccess,
        kParseJsonFailed,
        kErrorJsonKey,
    };

    using BinderList = std::vector<Binder>;

    JsonSerializer() : document_(), buffer_() {}
    ~JsonSerializer() {}

    inline std::string GetError() { return buffer_; }
    ParseResult Parse(const char* json, const BinderList& binder_list)
    {
        document_.Parse(json);
        if (document_.HasParseError())
            return ParseResult::kParseJsonFailed;
        for (const auto& v : binder_list)
        {
            if (!document_.HasMember(v.Key()))
            {
                buffer_ = "key not exist:";
                buffer_ += v.Key();
            }
            const rapidjson::Value& val = document_[v.Key()];
            switch (v.TypeId())
            {
            case Binder::Type::kInt:
                if (!val.IsInt())
                {
                    buffer_ = v.Key();
                    buffer_ += " is not a int";
                    return ParseResult::kErrorJsonKey;
                }
                v(val);
                break;
            case Binder::Type::kBool:
                THROW_NOT_IMPLEMENT
                break;
            case Binder::Type::kString:
                if (!val.IsString())
                {
                    buffer_ = v.Key();
                    buffer_ += " is not a string";
                    return ParseResult::kErrorJsonKey;
                }
                v(val);
                break;
            case Binder::Type::kNull:
                THROW_NOT_IMPLEMENT
                break;
            case Binder::Type::kArray:
                THROW_NOT_IMPLEMENT
                break;
            case Binder::Type::kObject:
                THROW_NOT_IMPLEMENT
                break;
            }
        }
        return ParseResult::kSuccess;
    }

protected:
    ::rapidjson::Document document_;
    std::string buffer_;
};

JSON_SERIALIZER_END

#endif