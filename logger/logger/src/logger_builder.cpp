#include "../include/logger_builder.h"

logger::severity logger_builder::string_to_severity(
    std::string const &severity_string)
{
    if (severity_string == "TRACE")
    {
        return logger::severity::trace;
    }
    if (severity_string == "DEBUG")
    {
        return logger::severity::debug;
    }
    if (severity_string == "INFORMATION")
    {
        return logger::severity::information;
    }
    if (severity_string == "WARNING")
    {
        return logger::severity::warning;
    }
    if (severity_string == "ERROR")
    {
        return logger::severity::error;
    }
    if (severity_string == "CRITICAL")
    {
        return logger::severity::critical;
    }

    throw std::out_of_range("invalid severity string value");
}