#pragma once

#include <string>
#include <assert.h>

#include "queryAnswerType.h"
#include "queryDnsType.h"

struct queryResult
{
    time_t timeStamp;
    queryDnsType type;
    std::string domain;
    std::string client;
    queryAnswer answer;

    void parse(json_object* jsonObject)
    {
        const size_t jsonArrayLength = json_object_array_length(jsonObject);
        assert(jsonArrayLength == 8);

#ifdef _USE_32BIT_TIME_T
        timeStamp = atoi(json_object_get_string(json_object_array_get_idx(jsonObject, 0)));
#else
        timeStamp = atoll(json_object_get_string(json_object_array_get_idx(jsonObject, 0)));
#endif // _USE_32BIT_TIME_T
        type = convertStringToQueryDnsType(json_object_get_string(json_object_array_get_idx(jsonObject, 1)));
        domain = json_object_get_string(json_object_array_get_idx(jsonObject, 2));
        client = json_object_get_string(json_object_array_get_idx(jsonObject, 3));
        answer = convertStringToQueryAnswer(json_object_get_string(json_object_array_get_idx(jsonObject, 4)));
    }
};
