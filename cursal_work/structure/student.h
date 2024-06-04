//
// Created by Des Caldnd on 6/3/2024.
//

#ifndef MP_OS_STUDENT_H
#define MP_OS_STUDENT_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <mutex>
#include <format>

class student final
{
public:

    std::string _surname, _name;
    std::string _group;
    unsigned short _course;

    std::vector<std::pair<std::string, unsigned short>> _subjects;

    static std::mutex file_mut;
    static constexpr std::string path = "strings.str";

    void serialize(std::fstream& stream) const;

    size_t serialize_size() const noexcept;

    static student deserialize(std::fstream& stream);

    nlohmann::json to_json() const;

    static student from_json(const nlohmann::json& json);

    std::string to_string() const;
};

class cw_string : public std::string
{
public:

    void serialize(std::fstream& stream) const;

    size_t serialize_size() const noexcept;

    static cw_string deserialize(std::fstream& stream);

    nlohmann::json to_json() const;

    static cw_string from_json(const nlohmann::json& json);

    std::string to_string() const;
};


#endif //MP_OS_STUDENT_H
