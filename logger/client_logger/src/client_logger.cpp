#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include "../include/client_logger.h"

std::unordered_map<std::string, std::pair<size_t, std::ofstream>> client_logger::refcounted_stream::_global_streams;

//client_logger::client_logger(
//    client_logger const &other)
//{
//    throw not_implemented("client_logger::client_logger(client_logger const &other)", "your code should be here...");
//}
//
//client_logger &client_logger::operator=(
//    client_logger const &other)
//{
//    throw not_implemented("client_logger &client_logger::operator=(client_logger const &other)", "your code should be here...");
//}
//
//client_logger::client_logger(
//    client_logger &&other) noexcept
//{
//    throw not_implemented("client_logger::client_logger(client_logger &&other) noexcept", "your code should be here...");
//}
//
//client_logger &client_logger::operator=(
//    client_logger &&other) noexcept
//{
//    throw not_implemented("client_logger &client_logger::operator=(client_logger &&other) noexcept", "your code should be here...");
//}
//
//client_logger::~client_logger() noexcept
//{
//    throw not_implemented("client_logger::~client_logger() noexcept", "your code should be here...");
//}

logger const *client_logger::log(
    const std::string &text,
    logger::severity severity) const
{
    std::string output = make_format(text, severity);

    auto it = _output_streams.find(severity);

    if (it == _output_streams.end())
        return this;

    if (it->second.second)
    {
        std::cout << output << std::endl;
    }

    std::ranges::for_each(it->second.first, [&output](std::ofstream* ofstr){
        if (ofstr != nullptr)
            *ofstr << output << std::endl;
        }, [](const refcounted_stream& stream){
        return stream._stream.second;
    });

    return this;
}

std::string client_logger::make_format(const std::string &message, severity sev) const
{
    std::ostringstream res;

    for(auto it = _format.begin(), end = _format.end(); it != end; ++it)
    {
        flag type = flag::NO_FLAG;
        if (*it == '%')
            type = char_to_flag(*(it + 1));

        if (type != flag::NO_FLAG)
        {
            switch (type)
            {
                case flag::DATE:
                    res << current_date_to_string();
                    break;
                case flag::TIME:
                    res << current_time_to_string();
                    break;
                case flag::SEVERITY:
                    res << severity_to_string(sev);
                    break;
                default:
                    res << message;
                    break;
            }
            ++it;
        } else
        {
            res << *it;
        }
    }

    return res.str();
}

client_logger::client_logger(
        const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>> &streams,
        std::string format) : _output_streams(streams), _format(std::move(format)){}

client_logger::flag client_logger::char_to_flag(char c) noexcept
{
    switch (c)
    {
        case 'd':
            return flag::DATE;
        case 't':
            return flag::TIME;
        case 's':
            return flag::SEVERITY;
        case 'm':
            return flag::MESSAGE;
        default:
            return flag::NO_FLAG;
    }
}

client_logger::refcounted_stream::refcounted_stream(const std::string &path) : _stream(std::make_pair(path, nullptr)) {}

client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &oth) : _stream(std::make_pair(oth._stream.first,
                                                                                                                          nullptr))
{
    auto it = _global_streams.find(oth._stream.first);

    if (it != _global_streams.end())
    {
        ++it->second.first;
        _stream.second = &it->second.second;
    } else
    {
        auto inserted = _global_streams.emplace(_stream.first, std::make_pair<size_t>(1, std::ofstream(oth._stream.first)));

        if (!inserted.second || !inserted.first->second.second.is_open())
        {
            if (inserted.second)
            {
                _global_streams.erase(inserted.first);
            }
            throw std::ios_base::failure("File " + oth._stream.first + " could not be opened");
        }

        _stream.second = &inserted.first->second.second;
    }
}

client_logger::refcounted_stream &
client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &oth)
{
    if (this == &oth)
        return *this;

    if (_stream.second != nullptr)
    {
        auto it = _global_streams.find(_stream.first);

        if (it != _global_streams.end())
        {
            --it->second.first;

            if (it->second.first == 0)
            {
                it->second.second.close();
                _global_streams.erase(it);
            }
        }
    }

    _stream.first = oth._stream.first;
    _stream.second = oth._stream.second;

    if (_stream.second != nullptr)
    {
        auto it = _global_streams.find(_stream.first);
        ++it->second.first;
    }

    return *this;
}

client_logger::refcounted_stream::refcounted_stream(client_logger::refcounted_stream &&oth) noexcept
{
    std::swap(_stream.first, oth._stream.first);
    _stream.second = std::exchange(oth._stream.second, nullptr);
}

client_logger::refcounted_stream &client_logger::refcounted_stream::operator=(client_logger::refcounted_stream &&oth) noexcept
{
    if (this == &oth)
        return *this;

    std::swap(_stream.first, oth._stream.first);
    std::swap(_stream.second, oth._stream.second);

    return *this;
}

client_logger::refcounted_stream::~refcounted_stream()
{
    if (_stream.second != nullptr)
    {
        auto it = _global_streams.find(_stream.first);

        if (it != _global_streams.end())
        {
            --it->second.first;

            if (it->second.first == 0)
            {
                it->second.second.close();
                _global_streams.erase(it);
            }
        }
    }
}
