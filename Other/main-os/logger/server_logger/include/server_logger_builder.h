#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H

#include <logger_builder.h>
#include "server_logger.h"

#include <map>
#include <unordered_map>
#include <iostream>
#include <fstream>


class server_logger_builder final:
    public logger_builder
{
private:
    std::unordered_map<server_logger::severity, server_logger::logger_subscription> subscriptions;
public:

    server_logger_builder();

    server_logger_builder(
        server_logger_builder const &other);

    server_logger_builder &operator=(
        server_logger_builder const &other);

    server_logger_builder(
        server_logger_builder &&other) noexcept;

    server_logger_builder &operator=(
        server_logger_builder &&other) noexcept;

    ~server_logger_builder() noexcept override;

public:

    logger_builder *add_file_stream(
        std::string const &stream_file_path,
        logger::severity severity) override;

    logger_builder *add_console_stream(
        logger::severity severity) override;

    logger_builder* transform_with_configuration(
        std::string const &configuration_file_path,
        std::string const &configuration_path) override;

    logger_builder *clear() override;

    [[nodiscard]] logger *build() const override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H