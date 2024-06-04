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
#include <fstream>


class cw_string : public std::string
{
public:

    cw_string() =default;

    cw_string(const std::string& other);
    cw_string(std::string&& other);

    cw_string& operator=(const std::string& other);
    cw_string& operator=(std::string&& other);

    void serialize(std::fstream& stream) const;

    size_t serialize_size() const noexcept;

    static cw_string deserialize(std::fstream& stream);

    nlohmann::json to_json() const;

    static cw_string from_json(const nlohmann::json& json);

    std::string to_string() const;

    static std::mutex file_mut;
    static constexpr std::string path = "strings.str";
};

class student final
{
public:

    cw_string _surname, _name;
    cw_string _group;
    unsigned short _course;

    std::vector<std::pair<cw_string, unsigned short>> _subjects;


    void serialize(std::fstream& stream) const;

    size_t serialize_size() const noexcept;

    static student deserialize(std::fstream& stream);

    nlohmann::json to_json() const;

    static student from_json(const nlohmann::json& json);

    std::string to_string() const;
};



#endif //MP_OS_STUDENT_H
