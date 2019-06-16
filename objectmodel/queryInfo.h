#pragma once

#include <assert.h>
#include <map>
#include <vector>

struct queryInfo
{
    std::vector<queryResult> data;
    std::map<std::string, unsigned int> hitPerDomain;
    std::map<std::string, time_t> lastForwardPerDomain;

    void parse(const std::string& buffer)
    {
        json_object* jsonObject = json_tokener_parse(buffer.c_str());
        const json_type type = json_object_get_type(jsonObject);

        assert(type == json_type_object);
        json_object_object_foreach(jsonObject, key, val)
        {
            if (strcmp(key, "data") == 0)
            {
                const json_type type = json_object_get_type(val);
                assert(type == json_type_array);

                json_object* jsonArray = json_object_object_get(jsonObject, key); //Getting the array if it is a key value pair

                const size_t jsonArrayLength = json_object_array_length(jsonArray);
                for (int i = 0; i < jsonArrayLength; ++i)
                {
                    json_object* jsonArrayEntry = json_object_array_get_idx(jsonArray, i);
                    const json_type entryType = json_object_get_type(jsonArrayEntry);

                    assert(type == json_type_array); //an object represented as an array of strings
                    queryResult result;
                    result.parse(jsonArrayEntry);
                    switch (result.queryAnswer)
                    {
                    case queryAnswer::Cached:
                    case queryAnswer::Forwarded:
                        data.push_back(result);
                        break;

                    default:
                        continue;
                    }

                    updateDomainHitStats(result.domain);
                    updateDomainLastForwardInfo(result);
                }
            }
        }
    }

    void updateDomainHitStats(const std::string& domain)
    {
        std::map<std::string, unsigned int>::iterator iter = hitPerDomain.find(domain);
        if (iter == hitPerDomain.end())
        {
            hitPerDomain[domain] = 1;
        }
        else
        {
            ++iter->second;
        }
    }

    void updateDomainLastForwardInfo(const queryResult& queryResult)
    {
        std::map<std::string, time_t>::iterator iter = lastForwardPerDomain.find(queryResult.domain);
        if (iter == lastForwardPerDomain.end())
        {
            lastForwardPerDomain[queryResult.domain] = queryResult.timeStamp;
        }
        else
        {
            if (queryResult.timeStamp > iter->second)
            {
                iter->second = queryResult.timeStamp;
            }
        }
    }
};