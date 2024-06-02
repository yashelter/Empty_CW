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

logger_builder* server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path)
{
    throw not_implemented("logger_builder* server_logger_builder::transform_with_configuration(std::string const &configuration_file_path, std::string const &configuration_path)", "invalid cal");
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
