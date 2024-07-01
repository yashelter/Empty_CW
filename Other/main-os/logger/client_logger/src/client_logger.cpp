#include <not_implemented.h>

#include "../include/client_logger.h"
#include <sstream>

#include <fstream>
//#include <utility> 

std::unordered_map<std::string, std::pair<std::ofstream, size_t>> client_logger::_all_streams;

client_logger::client_logger(std::unordered_map<severity, logger_subscription> subs, std::string format):subscriptions(subs), _format(format)
{
    for (const auto& pair : subscriptions)
    {
        logger_subscription my_sub = pair.second;

        for (std::string path : my_sub._paths)
        {
            auto iter = _all_streams.find(path);
            if (iter == _all_streams.end())
            {
                auto state = _all_streams.emplace(path, std::make_pair(std::ofstream(path), 0));
                if (!state.second || !state.first->second.first.is_open())
                {
                    throw std::runtime_error("File " + path + " could not be opened");
                }
                iter = state.first;
            }
            ++(iter->second.second);
        }
    }
}

logger const* client_logger::log(
    const std::string& mes,
    logger::severity severity) const noexcept
{
    std::string text = make_format(mes, severity);

    auto iter = subscriptions.find(severity);
    if (iter == subscriptions.end())
    {
        return this;
    }
    if (iter->second._console_stream)
    {
        std::cout << text << std::endl;
    }
    for (auto &path : iter->second._paths)
    {
        auto state = _all_streams.find(path);
        if (state != _all_streams.end())
        {
            (state->second.first) << text << std::endl;
        }
    }
   

    return this;
}


std::string client_logger::make_format(const std::string& message, logger::severity severity) const
{

    std::stringstream res;
    bool next_checkable = false;
    for (int i = 0; i < _format.size(); ++i)
    { 
        char symb = _format[i];
        if (symb == '%' && !next_checkable)
        {
            next_checkable = true;
            continue;
        }
        if (next_checkable)
        {
            next_checkable = false;
            if (symb == 'd')
            {
                res << current_date_to_string();
            }
            else if (symb == 't')
            {
                res << current_date_to_string();
            }
            else if(symb == 'm')
            {
                res << message;
            }
            else if (symb == 's')
            {
                res << severity_to_string(severity);
            }
            else
            {
                res << '%';
                next_checkable = true;
            }

        } else
        {
            res << symb;
        }
    }
    return res.str();
}


client_logger::client_logger(
    client_logger const &other)
{
    throw not_implemented("client_logger::client_logger(client_logger const &other)", "your code should be here...");
}

client_logger &client_logger::operator=(
    client_logger const &other)
{
    throw not_implemented("client_logger &client_logger::operator=(client_logger const &other)", "your code should be here...");
}

client_logger::client_logger(
    client_logger &&other) noexcept
{
    throw not_implemented("client_logger::client_logger(client_logger &&other) noexcept", "your code should be here...");
}

client_logger &client_logger::operator=(
    client_logger &&other) noexcept
{
    throw not_implemented("client_logger &client_logger::operator=(client_logger &&other) noexcept", "your code should be here...");
}

client_logger::~client_logger()
{
    for (auto &k_w : subscriptions)
    {
        for (std::string path: k_w.second._paths)
        {
            auto iter = _all_streams.find(path);
            if (iter != _all_streams.end())
            {
                iter->second.second -= 1;
                if (iter->second.second <= 0)
                {
                    iter->second.first.close();
                    _all_streams.erase(iter->first);
                }
            }
        }
        
    }
}
