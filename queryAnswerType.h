#pragma once

enum class queryAnswer
{
    Unknown,
    Blocked,
    Forwarded,
    Cached,
    WildCardBlocked
};

queryAnswer convertStringToQueryAnswer(const char* value)
{
    return static_cast<queryAnswer>(atoi(value));
}

queryAnswer convertStringToQueryAnswer(const std::string& value)
{
    return convertStringToQueryAnswer(value.c_str());
}