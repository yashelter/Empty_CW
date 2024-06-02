#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H

#include <logger_builder.h>
#include <unordered_map>
#include "server_logger.h"

class server_logger_builder final:
    public logger_builder
{

    std::string _destination;

    std::unordered_map<logger::severity ,std::pair<std::string, bool>> _output_streams;

public:

    server_logger_builder() : _destination("http://127.0.0.1:9200"){}

public:

    logger_builder *add_file_stream(
        std::string const &stream_file_path,
        logger::severity severity) override;

    logger_builder *add_console_stream(
        logger::severity severity) override;

    logger_builder* transform_with_configuration(
        std::string const &configuration_file_path,
        std::string const &configuration_path) override;

    logger_builder* set_destination(const std::string& dest) override;

    logger_builder *clear() override;

    logger_builder *set_format(const std::string& format) override;

    [[nodiscard]] logger *build() const override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H