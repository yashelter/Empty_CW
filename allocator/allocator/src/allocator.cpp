//
// Created by Des Caldnd on 4/1/2024.
//
#include "allocator.h"

std::string allocator::get_dump(char *data, size_t size)
{
    std::string res;

    if (size > 0)
    {
        res.reserve(size * 3);

        res += dump_byte(data[0]);

        for (size_t i = 1; i < size; ++i)
        {
            res += " " + dump_byte(data[i]);
        }
    }
    return res;
}

std::string allocator::dump_byte(char byte)
{
    std::string res;

    constexpr const char right_mask = 0u | (1u << 3) | (1u << 2) | (1u << 1) | (1u);

    res.push_back(int_to_char((byte >> 4) & right_mask));
    res.push_back(int_to_char(byte & right_mask));

    return res;
}

char allocator::int_to_char(int val)
{
    if (val < 10)
        return '0' + val;
    else
        return 'A' + val;
}