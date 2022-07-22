#ifndef __JSON_SERIALIZER_PREFIX_H__
#define __JSON_SERIALIZER_PREFIX_H__

#define JSON_SERIALIZER_NAMESPACE_START \
    namespace jsonserializer            \
    {

#define JSON_SERIALIZER_NAMESPACE_END }
#define THROW_NOT_IMPLEMENT throw std::exception("not implement yet");

enum class ValueType : int
{
    kInt,
    kString,
    kBool,
    kObject,
    kArray,
};

#endif