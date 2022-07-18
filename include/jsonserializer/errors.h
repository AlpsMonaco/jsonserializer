#ifndef __JSON_SERIALIZER_ERRORS_H__
#define __JSON_SERIALIZER_ERRORS_H__

#include "prefix.h"
#include <memory>
#include <string>

JSON_SERIALIZER_NAMESPACE_START

class Errors
{
public:
    Errors() : msg_pointer(), has_error_(false) {}
    Errors(const std::string& msg) : msg_pointer(std::make_unique<std::string>(msg)), has_error_(true) {}
    Errors(std::string&& msg) : msg_pointer(std::make_unique<std::string>(std::move(msg))), has_error_(true) {}
    Errors(const Errors& errors) : msg_pointer(), has_error_(errors.has_error_)
    {
        if (has_error_)
            msg_pointer = std::make_unique<std::string>(*errors.msg_pointer);
    }
    Errors(Errors&& errors) : msg_pointer(), has_error_(errors.has_error_)
    {
        if (has_error_)
            msg_pointer = std::move(errors.msg_pointer);
    }
    ~Errors() {}

    operator bool() { return has_error_; }

    Errors& operator=(const Errors& errors)
    {
        if (has_error_)
            msg_pointer.release();
        has_error_ = errors.has_error_;
        if (has_error_)
            msg_pointer = std::make_unique<std::string>(*errors.msg_pointer);
        return *this;
    }

    Errors& operator=(Errors&& errors)
    {
        if (has_error_)
            msg_pointer.release();
        has_error_ = errors.has_error_;
        if (has_error_)
            msg_pointer = std::move(errors.msg_pointer);
        return *this;
    }

    inline const char* operator()() { return msg_pointer->c_str(); }

protected:
    std::unique_ptr<std::string> msg_pointer;
    bool has_error_;
};

JSON_SERIALIZER_NAMESPACE_END

#endif