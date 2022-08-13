#ifndef __JSON_SERIALIZER_PARSER_H__
#define __JSON_SERIALIZER_PARSER_H__

#include "prefix.h"
#include "error.h"

NAMESPACE_JSR_START

class Parser
{
public:
    template <typename JsonDomumentType, typename ObjectType>
    static Error Parse(const JsonDomumentType& d, const ObjectType& object)
    {
        Error error;
        for (const typename ObjectType::value_type& pair : object)
        {
            const std::string& key = pair.Key();
            if (!d.HasMember(key.c_str()))
            {
                error = Error(ErrorCode::kKeyNotExist);
                error += key;
                break;
            }
            const rapidjson::Value& value = d[key.c_str()];
            error = pair(value);
            if (error)
            {
                error += key;
                break;
            }
        }
        return error;
    }
};

NAMESPACE_JSR_END

#endif