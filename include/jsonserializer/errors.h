#ifndef __JSON_SERIALIZER_ERRORS_H__
#define __JSON_SERIALIZER_ERRORS_H__

#include "prefix.h"
#include <memory>
#include <string>
#include <vector>
#include <array>

JSON_SERIALIZER_NAMESPACE_START

class Errors
{
public:
    enum class ErrorCode : int
    {
        kNoError,
        kKeyNotExist,
        kNotAInt,
        kNotABool,
        kNotAString,
        kNotAObject,
        kErrorCodeNum,
    };

    Errors(ErrorCode error_code = ErrorCode::kNoError) : node_list_pointer_(
                                                             error_code != ErrorCode::kNoError ?
                                                                 std::make_unique<std::vector<std::string>>() :
                                                                 nullptr),
                                                         error_code_(error_code)
    {
    }

    Errors(const Errors& errors, const char* node_name)
        : error_code_(errors.error_code_),
          node_list_pointer_(
              std::make_unique<std::vector<std::string>>(
                  *errors.node_list_pointer_))
    {
        node_list_pointer_->emplace_back(node_name);
    }
    Errors(Errors&& errors, const char* node_name)
        : error_code_(errors.error_code_),
          node_list_pointer_(std::move(errors.node_list_pointer_))
    {
        node_list_pointer_->emplace_back(node_name);
    }
    Errors(const Errors& errors) : node_list_pointer_(), error_code_(errors.error_code_)
    {
        if (*this)
            node_list_pointer_ =
                std::make_unique<std::vector<std::string>>(*errors.node_list_pointer_);
    }
    Errors(Errors&& errors) : node_list_pointer_(), error_code_(errors.error_code_)
    {
        if (*this)
            node_list_pointer_ =
                std::make_unique<std::vector<std::string>>(std::move(*errors.node_list_pointer_));
    }
    ~Errors() {}

    operator bool() { return error_code_ > ErrorCode::kNoError; }

    Errors& operator=(const Errors& errors)
    {
        if (*this)
            node_list_pointer_.release();
        error_code_ = errors.error_code_;
        if (*this)
            node_list_pointer_ =
                std::make_unique<std::vector<std::string>>(*errors.node_list_pointer_);
        return *this;
    }

    Errors& operator=(Errors&& errors)
    {
        if (*this)
            node_list_pointer_.release();
        error_code_ = errors.error_code_;
        if (*this)
            node_list_pointer_ = std::move(errors.node_list_pointer_);
        return *this;
    }

    inline std::string operator()()
    {
        std::string result;
        for (auto it = node_list_pointer_->rbegin(); it < node_list_pointer_->rend(); it++)
        {
            result += *it;
            result += ".";
        }
        if (result.size() > 0)
        {
            result = "key \"" + result;
            result[result.size() - 1] = '"';
            result += " ";
        }
        return result + ErrorDict::Get().Query(error_code_);
    }

protected:
    class ErrorDict
    {
        ErrorDict() : error_code_map_()
        {
            Set(ErrorCode::kKeyNotExist, "not exist");
            Set(ErrorCode::kNotAInt, "is not a int");
            Set(ErrorCode::kNotABool, "is not a bool");
            Set(ErrorCode::kNotAString, "is not a string");
            Set(ErrorCode::kNotAObject, "is not a object");
        }
        std::array<const char*,
                   static_cast<size_t>(ErrorCode::kErrorCodeNum)>
            error_code_map_;

        inline void Set(ErrorCode error_code, const char* msg) { error_code_map_[static_cast<int>(error_code)] = msg; }

    public:
        ~ErrorDict() {}
        static ErrorDict& Get()
        {
            static ErrorDict error_dict;
            return error_dict;
        }

        inline const char* Query(ErrorCode error_code) { return error_code_map_[static_cast<int>(error_code)]; }
    };
    ErrorCode error_code_;
    std::unique_ptr<std::vector<std::string>> node_list_pointer_;
};

JSON_SERIALIZER_NAMESPACE_END

#endif