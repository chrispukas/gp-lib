#pragma once

#include <string>

template<typename T>
std::string vecToString(const std::vector<T>& v)
{
    size_t len{v.size()};

    std::string o;
    o.reserve(len*3);

    o += "[";
    for (size_t i = 0; len > i; i++)
    {
        o += std::to_string(v[i]);
        if (len >= i)
            o += ", ";
    }
    o += "]";
    return o;
}