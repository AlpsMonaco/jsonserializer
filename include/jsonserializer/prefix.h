#ifndef __JSON_SERIALIZER_PREFIX_H__
#define __JSON_SERIALIZER_PREFIX_H__

#define JSON_SERIALIZER_NAMESPACE_START \
    namespace jsonserializer            \
    {

#define JSON_SERIALIZER_NAMESPACE_END }


#ifdef _WIN32
#include <exception>
#define THROW_NOT_IMPLEMENT throw std::exception("not implement yet");
#else
#define THROW_NOT_IMPLEMENT
#endif

enum class ValueType : int
{
    kInt,
    kInt64,
    kDouble,
    kString,
    kBool,
    kObject,
    kArray,
};

#endif