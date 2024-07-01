#include "../include/logger.h"
#include <iomanip>
#include <sstream>

logger const *logger::trace(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::trace);
}

logger const *logger::debug(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::debug);
}

logger const *logger::information(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::information);
}

logger const *logger::warning(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::warning);
}

logger const *logger::error(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::error);
}

logger const *logger::critical(
    std::string const &message) const noexcept
{
    return log(message, logger::severity::critical);
}

std::string logger::severity_to_string(
    logger::severity severity)
{
    switch (severity)
    {
        case logger::severity::trace:
            return "TRACE";
        case logger::severity::debug:
            return "DEBUG";
        case logger::severity::information:
            return "INFORMATION";
        case logger::severity::warning:
            return "WARNING";
        case logger::severity::error:
            return "ERROR";
        case logger::severity::critical:
            return "CRITICAL";
    }

    throw std::out_of_range("Invalid severity value");
}

logger::severity logger::string_to_severity(const std::string& str)
{
    if (str == "TRACE")
    {
        return logger::severity::trace;
    } else if (str == "DEBUG")
    {
        return logger::severity::debug;
    } else if (str == "INFORMATION")
    {
        return logger::severity::information;
    } else if (str == "WARNING")
    {
        return logger::severity::warning;
    } else if (str == "ERROR")
    {
        return logger::severity::error;
    } else if (str == "CRITICAL")
    {
        return logger::severity::critical;
    } else
    {
        throw std::invalid_argument("Invalid severity string");
    }
}

std::string logger::current_datetime_to_string() noexcept
{
    auto time = std::time(nullptr);

    std::ostringstream result_stream;
    result_stream << std::put_time(std::localtime(&time), "%d.%m.%Y %H:%M:%S");

    return result_stream.str();
}

std::string logger::current_date_to_string() noexcept
{
    auto time = std::time(nullptr);

    std::ostringstream result_stream;
    result_stream << std::put_time(std::localtime(&time), "%d.%m.%Y");

    return result_stream.str();
}

std::string logger::current_time_to_string() noexcept
{
    auto time = std::time(nullptr);

    std::ostringstream result_stream;
    result_stream << std::put_time(std::localtime(&time), "%H:%M:%S");

    return result_stream.str();
}