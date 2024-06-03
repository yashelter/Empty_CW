#include <not_implemented.h>
#include "../include/server_logger_builder.h"

logger_builder *server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    auto it = _output_streams.find(severity);

    if (it == _output_streams.end())
    {
        it = _output_streams.emplace(severity, std::make_pair(std::string(), false)).first;
    }

    it->second.first = stream_file_path;

    return this;
}

logger_builder *server_logger_builder::add_console_stream(
    logger::severity severity)
{
    auto it = _output_streams.find(severity);

    if (it == _output_streams.end())
    {
        it = _output_streams.emplace(severity, std::make_pair(std::string(), false)).first;
    }

    it->second.second = true;

    return this;
}

using namespace nlohmann;

logger_builder* server_logger_builder::transform_with_configuration(
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

    return this;
}

logger_builder *server_logger_builder::clear()
{
    _output_streams.clear();
    _destination = "http://127.0.0.1:9200";

    return this;
}

logger *server_logger_builder::build() const
{
    return new server_logger(_destination, _output_streams);
}

logger_builder *server_logger_builder::set_destination(const std::string& dest)
{
    _destination = dest;

    return this;
}

logger_builder *server_logger_builder::set_format(const std::string &format)
{
    throw not_implemented("logger *server_logger_builder::set_format() const", "invalid call");
}

void server_logger_builder::parse_severity(logger::severity sev, json &j)
{
    if (j.empty() || !j.is_object())
        return;

    auto it = _output_streams.find(sev);

    auto data_it = j.find("path");
    if (data_it != j.end() && data_it->is_string())
    {
        std::string data = *data_it;

        auto console_it = j.find("console");

        bool console = false;

        if (console_it != j.end() && console_it->is_boolean())
            console = *console_it;

        if (it == _output_streams.end())
        {
            it = _output_streams.emplace(sev, std::make_pair(data, console)).first;
        }
    }
}
