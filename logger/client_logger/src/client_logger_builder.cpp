#include <filesystem>
#include <utility>
#include <not_implemented.h>
#include "../include/client_logger_builder.h"

using namespace nlohmann;

logger_builder *client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    auto it = _output_streams.find(severity);
    if (it == _output_streams.end())
    {
        auto inserted = _output_streams.emplace(severity, std::make_pair(std::forward_list<client_logger::refcounted_stream>(), false));
        it = inserted.first;
    }

    it->second.first.emplace_front(std::filesystem::weakly_canonical(stream_file_path).string());

    return this;
}

logger_builder *client_logger_builder::add_console_stream(
    logger::severity severity)
{
    auto it = _output_streams.find(severity);
    if (it == _output_streams.end())
    {
        auto inserted = _output_streams.emplace(severity, std::make_pair(std::forward_list<client_logger::refcounted_stream>(), true));
        it = inserted.first;
    }

    it->second.second = true;

    return this;
}

logger_builder* client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path)
{
    std::ifstream file(configuration_file_path);

    if (!file.is_open())
        throw std::ios_base::failure("File " + configuration_file_path + " could not be opened");

    json data = json::parse(file);

    file.close();

    auto it = data.find(configuration_path);

    if (it == data.end() || !it->is_object())
        return this;

    parse_severity(logger::severity::trace, (*it)["trace"]);
    parse_severity(logger::severity::debug, (*it)["debug"]);
    parse_severity(logger::severity::information, (*it)["information"]);
    parse_severity(logger::severity::warning, (*it)["warning"]);
    parse_severity(logger::severity::error, (*it)["error"]);
    parse_severity(logger::severity::critical, (*it)["critical"]);

    auto format = it->find("format");

    if (format != it->end() && format->is_string())
    {
        _format = format.value();
    }

    return this;
}

logger_builder *client_logger_builder::clear()
{
    _output_streams.clear();
    _format = "%m";

    return this;
}

logger *client_logger_builder::build() const
{
    return new client_logger(_output_streams, _format);
}

logger_builder *client_logger_builder::set_format(const std::string &format)
{
    _format = format;
    return this;
}

void client_logger_builder::parse_severity(logger::severity sev, nlohmann::json& j)
{
    if (j.empty() || !j.is_object())
        return;

    auto it = _output_streams.find(sev);

    auto data_it = j.find("paths");
    if (data_it != j.end() && data_it->is_array())
    {
        json data = *data_it;

        for (const json& js: data)
        {
            if (js.empty() || !js.is_string())
                continue;
            const std::string& path = js;
            if (it == _output_streams.end())
            {
                it = _output_streams.emplace(sev, std::make_pair(std::forward_list<client_logger::refcounted_stream>(),
                                                                 false)).first;
            }

            it->second.first.emplace_front(std::filesystem::weakly_canonical(path).string());
        }
    }

    auto console = j.find("console");

    if (console != j.end() && console->is_boolean())
    {
        if (it == _output_streams.end())
        {
            it = _output_streams.emplace(sev, std::make_pair(std::forward_list<client_logger::refcounted_stream>(), false)).first;
        }
        it->second.second = console->get<bool>();
    }
}

logger_builder *client_logger_builder::set_destination(const std::string &format)
{
    throw not_implemented("logger_builder *client_logger_builder::set_destination(const std::string &format)", "invalid call");
}
