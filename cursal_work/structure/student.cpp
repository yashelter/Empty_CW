//
// Created by Des Caldnd on 6/3/2024.
//

#include "student.h"

void cw_string::serialize(std::fstream &stream) const
{

}

size_t cw_string::serialize_size() const noexcept
{
    return 0;
}

cw_string cw_string::deserialize(std::fstream &stream)
{
    return cw_string();
}

nlohmann::json cw_string::to_json() const
{
    return nlohmann::json();
}

cw_string cw_string::from_json(const nlohmann::json &json)
{
    return cw_string();
}

std::string cw_string::to_string() const
{
    return std::string();
}

std::string student::to_string() const
{
    return std::string();
}

student student::from_json(const nlohmann::json &json)
{
    return student();
}

nlohmann::json student::to_json() const
{
    return nlohmann::json();
}

student student::deserialize(std::fstream &stream)
{
    return student();
}

size_t student::serialize_size() const noexcept
{
    return 0;
}

void student::serialize(std::fstream &stream) const
{

}
