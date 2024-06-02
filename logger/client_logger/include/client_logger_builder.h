#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H

#include <logger_builder.h>
#include <unordered_map>
#include <forward_list>
#include <nlohmann/json.hpp>
#include "client_logger.h"

class client_logger_builder final:
    public logger_builder
{
private:

    std::unordered_map<logger::severity ,std::pair<std::forward_list<client_logger::refcounted_stream>, bool>> _output_streams;

    std::string _format;

    void parse_severity(logger::severity, nlohmann::json& j);

public:

    client_logger_builder() : _format("%m"){};

    client_logger_builder(
        client_logger_builder const &other) =delete;

    client_logger_builder &operator=(
        client_logger_builder const &other) =delete;

    client_logger_builder(
        client_logger_builder &&other) noexcept =default;

    client_logger_builder &operator=(
        client_logger_builder &&other) noexcept =default;

    ~client_logger_builder() noexcept override =default;

public:

    logger_builder *add_file_stream(
        std::string const &stream_file_path,
        logger::severity severity) override;

    logger_builder *add_console_stream(
        logger::severity severity) override;

    logger_builder* transform_with_configuration(
        std::string const &configuration_file_path,
        std::string const &configuration_path) override;

    logger_builder *set_format(const std::string& format) override;

    virtual logger_builder *set_destination(const std::string& format) override;

    logger_builder *clear() override;

    [[nodiscard]] logger *build() const override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H