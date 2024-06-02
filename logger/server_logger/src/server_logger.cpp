#include <not_implemented.h>
#include <httplib.h>
#include "../include/server_logger.h"

#ifdef _WIN32
#include <process.h>
#elif
#include <unistd.h>
#endif

server_logger::~server_logger() noexcept
{
    httplib::Params par;
    par.emplace("pid", std::to_string(server_logger::inner_getpid()));
    _client.Get("/destroy", par, httplib::Headers());
}

logger const *server_logger::log(
    const std::string &text,
    logger::severity severity) const noexcept
{
    std::stringstream stringstream;

    stringstream << "[" << current_date_to_string() << " " << current_time_to_string() <<
            "][" << severity_to_string(severity) << "] " << text;

    httplib::Params par;
    par.emplace("pid", std::to_string(server_logger::inner_getpid()));
    par.emplace("sev", severity_to_string(severity));
    par.emplace("message", stringstream.str());

    _client.Get("/log", par, httplib::Headers());
    return this;
}

server_logger::server_logger(const std::string& dest,
                             const std::unordered_map<logger::severity, std::pair<std::string, bool>> &streams)
                             : _client(dest)
{
    for (auto& pair : streams)
    {
        httplib::Params par;
        par.emplace("pid", std::to_string(server_logger::inner_getpid()));
        par.emplace("sev", severity_to_string(pair.first));
        par.emplace("path", pair.second.first);
        par.emplace("console", pair.second.second ? "1" : "0");

        _client.Get("/init", par, httplib::Headers());
    }
}

int server_logger::inner_getpid()
{
#ifdef _WIN32
    return ::_getpid();
#elif
    return getpid();
#endif
}
