#ifndef __JSON_SERIALIZER_ERRORS_H__
#define __JSON_SERIALIZER_ERRORS_H__

#include "prefix.h"
#include <memory>
#include <string>
#include <vector>
#include <array>

JSON_SERIALIZER_NAMESPACE_START

class Error
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
        kNotAnArray,
        kArrayTypeNotMatch,
        kErrorCodeNum,
    };

    using NodeList        = std::vector<std::string>;
    using NodeListPointer = std::unique_ptr<NodeList>;

    Error(ErrorCode error_code = ErrorCode::kNoError) : node_list_pointer_(),
                                                        error_code_(error_code)
    {
    }

    Error(const Error& errors, const char* node_name)
        : error_code_(errors.error_code_),
          node_list_pointer_(
              errors.node_list_pointer_.get() != nullptr ?
                  std::make_unique<NodeList>(*errors.node_list_pointer_) :
                  std::make_unique<NodeList>())
    {
        node_list_pointer_->emplace_back(node_name);
    }
    Error(Error&& errors, const char* node_name)
        : error_code_(errors.error_code_),
          node_list_pointer_(
              errors.node_list_pointer_.get() != nullptr ?
                  std::move(errors.node_list_pointer_) :
                  std::make_unique<NodeList>())
    {
        node_list_pointer_->emplace_back(node_name);
    }
    Error(const Error& errors) : node_list_pointer_(), error_code_(errors.error_code_)
    {
        if (*this)
            node_list_pointer_ =
                std::make_unique<NodeList>(*errors.node_list_pointer_);
    }
    Error(Error&& errors) : node_list_pointer_(), error_code_(errors.error_code_)
    {
        if (*this)
            node_list_pointer_ =
                std::make_unique<NodeList>(std::move(*errors.node_list_pointer_));
    }
    ~Error() {}

    operator bool() { return error_code_ > ErrorCode::kNoError; }

    Error& operator=(const Error& errors)
    {
        if (*this)
            node_list_pointer_.release();
        error_code_ = errors.error_code_;
        if (*this)
            node_list_pointer_ =
                std::make_unique<NodeList>(*errors.node_list_pointer_);
        return *this;
    }

    Error& operator=(Error&& errors)
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
            result                    = "key \"" + result;
            result[result.size() - 1] = '"';
            result += " ";
        }
        return result + ErrorDict::Get().Query(error_code_);
    }

    inline std::string operator()() const { return const_cast<Error&>(*this)(); }
    inline ErrorCode Code() { return error_code_; }
    inline ErrorCode Code() const { return error_code_; }
    inline std::string Message() { return (*this)(); }
    inline std::string Message() const { return (*this)(); }
    inline const NodeList& GetNodeList() { return *node_list_pointer_; }
    inline const NodeList& GetNodeList() const { return *node_list_pointer_; }

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
            Set(ErrorCode::kNotAnArray, "is not an array");
            Set(ErrorCode::kArrayTypeNotMatch, "type of array dismatch");
        }
        std::array<const char*, static_cast<size_t>(ErrorCode::kErrorCodeNum)> error_code_map_;
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
    NodeListPointer node_list_pointer_;
};

JSON_SERIALIZER_NAMESPACE_END

#endif