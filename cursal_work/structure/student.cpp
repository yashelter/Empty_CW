#include "student.h"
#include <exception>
#include <sstream>
#include <ranges>
#include <algorithm>

std::mutex cw_string::file_mut;

void cw_string::serialize(std::fstream &stream) const
{
    size_t position;
    {
        std::lock_guard lock(file_mut);

        std::fstream real_file(path, std::ios_base::app | std::ios_base::binary | std::ios::out);

        if (!real_file.is_open())
            throw std::runtime_error("File for cw_string serialisation could not be opened");

        position = real_file.tellg();

        size_t length = size();

        real_file.write(reinterpret_cast<char *>(&length), sizeof(size_t));
        real_file.write(c_str(), length * sizeof(char));
    }

    stream.write(reinterpret_cast<char *>(&position), sizeof(size_t));
}

size_t cw_string::serialize_size() const noexcept
{
    return sizeof(size_t);
}

cw_string cw_string::deserialize(std::fstream &stream)
{
    cw_string res;

    size_t position;

    stream.read(reinterpret_cast<char *>(&position), sizeof(size_t));

    {
        std::lock_guard lock(file_mut);

        std::fstream real_file(path, std::ios_base::in | std::ios_base::binary);

        if (!real_file.is_open())
            throw std::runtime_error("File for cw_string deserialisation could not be opened");

        real_file.seekg(position, std::ios_base::beg);

        size_t length;

        real_file.read(reinterpret_cast<char*>(&length), sizeof (size_t));

        res.resize(length);

        real_file.read(res.data(), length * sizeof(char));
    }

    return res;
}

nlohmann::json cw_string::to_json() const
{
    return {static_cast<std::string>(*this)};
}

cw_string cw_string::from_json(const nlohmann::json &json)
{
    return {nlohmann::to_string(json)};
}

std::string cw_string::to_string() const
{
    return static_cast<std::string>(*this);
}


cw_string::cw_string(const std::string &other) : basic_string(other){}

cw_string::cw_string(std::string &&other) : basic_string(other){}

cw_string &cw_string::operator=(const std::string &other)
{
    std::string::operator=(other);
    return *this;
}

cw_string &cw_string::operator=(std::string &&other)
{
    std::string::operator=(other);
    return *this;
}

std::string student::to_string() const
{
    std::stringstream str;
    str << "Surname: " << _surname << ", Name: " << _name << ", Group: " << _group << ", Course: " << _course << "\n";

    for(auto& data : _subjects)
    {
        str << '\t' << data.first << ": " << data.second << "\n";
    }

    return str.str();
}

student student::from_json(const nlohmann::json &json)
{
    student res;

    auto surname = json.find("surname");

    if (surname == json.end() || !surname->is_string())
        throw std::runtime_error("Incorrect json for parsing student");

    res._surname = nlohmann::to_string(*surname);

    auto name = json.find("name");

    if (name == json.end() || !name->is_string())
        throw std::runtime_error("Incorrect json for parsing student");

    res._name = nlohmann::to_string(*name);

    auto group = json.find("group");

    if (group == json.end() || !group->is_string())
        throw std::runtime_error("Incorrect json for parsing student");

    res._group = nlohmann::to_string(*group);

    auto course = json.find("course");

    if (course == json.end() || !course->is_number_integer())
        throw std::runtime_error("Incorrect json for parsing student");

    res._course = course->get<int>();

    auto subjects = json.find("subjects");

    if (subjects == json.end() || !subjects->is_array())
        throw std::runtime_error("Incorrect json for parsing student");

    for(auto& j: *subjects)
    {
        auto n = j.find("subject");
        auto m = j.find("mark");

        if (n == j.end() || m == j.end() || !n->is_string() || !m->is_number_integer())
            throw std::runtime_error("Incorrect json for parsing student");

        res._subjects.emplace_back(nlohmann::to_string(*n), m->get<int>());
    }

    return res;
}

nlohmann::json student::to_json() const
{
    nlohmann::json res;

    res["surname"] = _surname.to_json();
    res["name"] = _name.to_json();
    res["group"] = _group.to_json();
    res["course"] = _course;

    nlohmann::json arr;

    for(auto& pair: _subjects)
    {
        nlohmann::json tmp;
        tmp["subject"] = pair.first.to_json();
        tmp["mark"] = pair.second;

        arr.push_back(tmp);
    }

    res["subjects"] = arr;

    return res;
}

student student::deserialize(std::fstream &stream)
{
    student res;

    res._surname = cw_string::deserialize(stream);
    res._name = cw_string::deserialize(stream);
    res._group = cw_string::deserialize(stream);

    stream.read(reinterpret_cast<char*>(&res._course), sizeof(unsigned short));

    size_t length;

    stream.read(reinterpret_cast<char*>(&length), sizeof(size_t));

    res._subjects.reserve(length);

    for(size_t i = 0; i < length; ++i)
    {
        auto str = cw_string::deserialize(stream);
        unsigned short mark;

        stream.read(reinterpret_cast<char*>(&mark), sizeof(unsigned short));

        res._subjects.emplace_back(std::move(str), mark);
    }


    return res;
}

size_t student::serialize_size() const noexcept
{
    return _surname.serialize_size() + _name.serialize_size() + _group.serialize_size() + sizeof(unsigned short) + sizeof(size_t) + std::accumulate(_subjects.begin(), _subjects.end(), 0ULL, [](size_t r, auto& pair){
        return r + pair.first.serialize_size() + sizeof(unsigned short);
    });
}

void student::serialize(std::fstream &stream) const
{
    _surname.serialize(stream);
    _name.serialize(stream);
    _group.serialize(stream);

    stream.write(reinterpret_cast<const char*>(&_course), sizeof(unsigned short));

    size_t length = _subjects.size();

    stream.write(reinterpret_cast<char*>(&length), sizeof(size_t));

    for(auto& pair : _subjects)
    {
        pair.first.serialize(stream);

        stream.write(reinterpret_cast<const char*>(&pair.second), sizeof(unsigned short));
    }
}
