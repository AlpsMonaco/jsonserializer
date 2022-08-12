#ifndef __JSON_SERIALIZER_ERROR_H__
#define __JSON_SERIALIZER_ERROR_H__

#include "prefix.h"
#include <ostream>

NAMESPACE_JSR_START

enum class ErrorCode : unsigned int
{
    kNoError,
    kParseJsonError,
    kKeyNotExist,
    kTypeMismatch,
    kTypeMismatchInArray,
    kErrorCodeSize,
};

using ErrorMessager = std::function<std::string(const std::vector<std::string>&)>;
class ErrorParser
{
    std::array<ErrorMessager, static_cast<size_t>(ErrorCode::kErrorCodeSize)> messager_list_;

    static std::string ParseKeyTree(const std::vector<std::string>& key_tree)
    {
        std::string key_chain;
        for (auto it = key_tree.rbegin(); it < key_tree.rend(); it++)
        {
            key_chain += *it;
            key_chain += ".";
        }
        if (key_chain.size() > 0)
            key_chain.erase(key_chain.end() - 1, key_chain.end());
        return key_chain;
    }

    static std::pair<std::string, std::string> ParseArrayKeyTree(const std::vector<std::string>& key_tree)
    {
        std::string key_chain;
        for (auto it = key_tree.rbegin(); it < key_tree.rend() - 1; it++)
        {
            key_chain += *it;
            key_chain += ".";
        }
        if (key_chain.size() > 0)
            key_chain.erase(key_chain.end() - 1, key_chain.end());
        return {key_tree[0], key_chain};
    }

    ErrorParser()
    {
        messager_list_[static_cast<size_t>(ErrorCode::kNoError)] = [](const std::vector<std::string>&) -> std::string
        {
            return "no error";
        };
        messager_list_[static_cast<size_t>(ErrorCode::kParseJsonError)] = [](const std::vector<std::string>&) -> std::string
        {
            return "parse json error";
        };
        messager_list_[static_cast<size_t>(ErrorCode::kKeyNotExist)] = [](const std::vector<std::string>& key_tree) -> std::string
        {
            return "the value in key \"" + ParseKeyTree(key_tree) + "\" not exist";
        };
        messager_list_[static_cast<size_t>(ErrorCode::kTypeMismatch)] = [](const std::vector<std::string>& key_tree) -> std::string
        {
            return "the value in key \"" + ParseKeyTree(key_tree) + "\" mismatch target type";
        };
        messager_list_[static_cast<size_t>(ErrorCode::kTypeMismatchInArray)] = [](const std::vector<std::string>& key_tree) -> std::string
        {
            auto p = ParseArrayKeyTree(key_tree);
            // return "the value in index " + p.first + " of array \"" + p.second + "\" type mismatch";
            return "type of the value mismatch in index "+ p.first + " of array \"" + p.second + "\"";;
        };
    }

    ~ErrorParser()
    {
    }

    ErrorMessager& operator[](size_t index)
    {
        return messager_list_[index];
    }

public:
    static std::string GetErrorMessage(ErrorCode error_code, const std::vector<std::string>& key_tree)
    {
        static ErrorParser error_parser;
        return error_parser[static_cast<size_t>(error_code)](key_tree);
    }
};

class Error
{
public:
    Error(ErrorCode error_code = ErrorCode::kNoError)
        : code_(error_code),
          key_tree_()
    {
    }

    Error(const Error& rhs)
        : code_(rhs.code_),
          key_tree_(rhs.key_tree_)
    {
    }

    Error(Error&& rhs)
        : code_(rhs.code_),
          key_tree_(std::move(rhs.key_tree_))
    {
    }

    Error& operator=(const Error& rhs)
    {
        code_ = rhs.code_;
        key_tree_ = rhs.key_tree_;
        return *this;
    }

    Error& operator=(Error&& rhs)
    {
        code_ = rhs.code_;
        key_tree_ = std::move(rhs.key_tree_);
        return *this;
    }

    ~Error() {}

    Error& AddKey(const std::string& key)
    {
        key_tree_.emplace_back(key);
        return *this;
    }

    Error& AddKey(std::string&& key)
    {
        key_tree_.emplace_back(std::move(key));
        return *this;
    }

    Error& operator+=(const std::string& key)
    {
        return AddKey(key);
    }

    Error& operator+=(std::string&& key)
    {
        return AddKey(std::move(key));
    }

    friend std::ostream& operator<<(std::ostream& os, const Error& error)
    {
        os << error.Message();
        return os;
    }

    operator bool() { return code_ != ErrorCode::kNoError; }
    unsigned int Code() { return static_cast<unsigned int>(code_); }
    std::string Message() { return ErrorParser::GetErrorMessage(code_, key_tree_); }
    const std::vector<std::string>& Keys() { return key_tree_; }

    const std::vector<std::string>& Keys() const { return const_cast<Error&>(*this).Keys(); }
    operator bool() const { return const_cast<Error&>(*this); }
    unsigned int Code() const { return const_cast<Error&>(*this).Code(); }
    std::string Message() const { return const_cast<Error&>(*this).Message(); }

protected:
    ErrorCode code_;
    std::vector<std::string> key_tree_;
};

NAMESPACE_JSR_END

#endif