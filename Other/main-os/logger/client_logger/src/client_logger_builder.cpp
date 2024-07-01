#include <not_implemented.h>

#include "../include/client_logger_builder.h"


client_logger_builder::client_logger_builder()
{
    _format = "%m";
}

client_logger_builder::client_logger_builder(
    client_logger_builder const& other) : subscriptions(other.subscriptions), _format(other._format)
{
   
}

client_logger_builder &client_logger_builder::operator=(
    client_logger_builder const &other)
{
    subscriptions = other.subscriptions;
    _format = other._format;
    return *this;
}

client_logger_builder::client_logger_builder(
    client_logger_builder&& other) noexcept
{
    subscriptions = std::move(other.subscriptions);
    _format = std::move(other._format);
}

client_logger_builder &client_logger_builder::operator=(
    client_logger_builder &&other) noexcept
{
    if (this != &other)
    {
        subscriptions = std::move(other.subscriptions);
        _format = std::move(other._format);
    }
    return *this;
}

client_logger_builder::~client_logger_builder() noexcept
{
    clear();
}

logger_builder *client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    auto iter = subscriptions.find(severity);
    if (iter == subscriptions.end())
    {
        auto answ = subscriptions.emplace(severity, client_logger::logger_subscription());
        if (!answ.second)
        {
            throw std::ios_base::failure("problem with emplace in hash_map\n");
        }
        iter = answ.first;
    }
    iter->second._paths.push_back(std::filesystem::weakly_canonical(stream_file_path).string());
    return this;
}

logger_builder *client_logger_builder::add_console_stream(
    logger::severity severity)
{
    auto iter = subscriptions.find(severity);
    if (iter == subscriptions.end())
    {
        auto answ = subscriptions.emplace(severity, client_logger::logger_subscription());
        if (!answ.second)
        {
            throw std::ios_base::failure("problem with emplace in hash_map\n");
        }
        iter = answ.first;
    }
    iter->second._console_stream = true;
    return this;
}

void client_logger_builder::set_format(std::string const& new_format)
{
    _format = new_format;
}

logger_builder *client_logger_builder::clear()
{
    subscriptions.clear();
    _format = "%m";
    return this;
}

client_logger *client_logger_builder::build() const
{
    return new client_logger(subscriptions, _format);
}


void client_logger_builder::parsing_severity(logger::severity severity, json& js)
{
    
    if (js.empty() || !js.is_object())
    {
        return;
    }
    auto console_iter = js.find("console");
    if (console_iter != js.end() && console_iter->is_boolean() && console_iter->get<bool>())
    {
        add_console_stream(severity);
    }

    auto path_iter = js.find("path");
    if (path_iter != js.end() && path_iter->is_array())
    {
        nlohmann::json arr = *path_iter;
        for (const nlohmann::json& obj : arr)
        {
            if (!obj.is_string() || obj.empty())
            {
                continue;
            }
            std::string path_file = obj.get<std::string>();
            add_file_stream(path_file, severity);
        }
    }
    

}


logger_builder* client_logger_builder::transform_with_configuration(
    std::string const& configuration_file_path,
    std::string const& configuration_path)
{
    std::ifstream file_json(configuration_file_path);
    if (!file_json.is_open())
    {
        throw std::ios_base::failure("problem with file " + configuration_file_path);
    }

    json json_str = json::parse(file_json);
    file_json.close();

    auto iter = json_str.find(configuration_path);
    if (iter == json_str.end() || !iter->is_object())
    {
        return this;
    }
    parsing_severity(logger::severity::information, (*iter)["information"]);
    parsing_severity(logger::severity::warning, (*iter)["warning"]);
    parsing_severity(logger::severity::error, (*iter)["error"]);
    parsing_severity(logger::severity::critical, (*iter)["critical"]);
    parsing_severity(logger::severity::debug, (*iter)["debug"]);
    parsing_severity(logger::severity::trace, (*iter)["trace"]);

    auto format = (*iter)["format"];

    if (!format.empty() && format.is_string())
    {
        _format = format.get<std::string>();
    }
   
    return this;

}
