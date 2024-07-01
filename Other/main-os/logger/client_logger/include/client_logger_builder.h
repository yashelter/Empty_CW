#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H

#include "client_logger.h"
#include <logger_builder.h>

#include <map>
#include <unordered_map>
#include <iostream>
#include <fstream>

class client_logger_builder final :
    public logger_builder
{
private:
    std::string _format;
    std::unordered_map<client_logger::severity, client_logger::logger_subscription> subscriptions;

public:
    void set_format(std::string const& new_format);

public:

    client_logger_builder();

    client_logger_builder(
        client_logger_builder const& other);

    client_logger_builder& operator=(
        client_logger_builder const& other);

    client_logger_builder(
        client_logger_builder&& other) noexcept;

    client_logger_builder& operator=(
        client_logger_builder&& other) noexcept;

    ~client_logger_builder() noexcept override;

public:

    logger_builder* add_file_stream(
        std::string const& stream_file_path,
        logger::severity severity) override;

    logger_builder* add_console_stream(
        logger::severity severity) override;

    logger_builder* transform_with_configuration(
        std::string const& configuration_file_path,
        std::string const& configuration_path) override;

    logger_builder* clear() override;

    [[nodiscard]] client_logger* build() const override;
private:
    void parsing_severity(logger::severity severity, json& js);
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H