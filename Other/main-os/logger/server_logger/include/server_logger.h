#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H

#include "../../logger/include/logger.h"
#include <filesystem>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "../tests/httplib.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

class server_logger final:
    public logger
{
private:
    mutable httplib::Client client;
public:
    class logger_subscription final
    {
        friend class server_logger_builder;
        friend class server_logger;
        friend class server;
    private:
        severity _severity;
        bool _console_stream = false;
        std::vector<std::string> _paths;
    };
public:
    server_logger(std::unordered_map<server_logger::severity, server_logger::logger_subscription>,
        const std::string& host, int port);
    server_logger &operator=(
        server_logger const &other);
    server_logger &operator=(
        server_logger &&other) noexcept;

    ~server_logger() noexcept final;

public:

    [[nodiscard]] logger const *log(
        const std::string &message,
        logger::severity severity) const noexcept override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H