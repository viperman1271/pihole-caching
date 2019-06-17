#pragma once

#include <string>
#include <assert.h>
#include <cstring>

enum class queryDnsType
{
    A,
    AAAA,
    PTR,
    Unknown,
};

queryDnsType convertStringToQueryDnsType(const char* value)
{
    assert(strlen(value) < 8);
    const unsigned long int* intValue = reinterpret_cast<const unsigned long int*>(value);
    switch (*intValue)
    {
        case 65: //"A"
            return queryDnsType::A;

        case 1094795585: //"AAAA"
            return queryDnsType::AAAA;

        default:
            if (strcmp(value, "A"))
            {
                return queryDnsType::A;
            }
            else if (strcmp(value, "AAAA"))
            {
                return queryDnsType::AAAA;
            }
            else if (strcmp(value, "PTR"))
            {
                return queryDnsType::PTR;
            }
            else
            {
                assert(false);
                return queryDnsType::Unknown;
            }
            break;
    }
    
}

queryDnsType convertStringToQueryDnsType(const std::string& value)
{
    const char* cString = value.c_str();
    return convertStringToQueryDnsType(cString);
}