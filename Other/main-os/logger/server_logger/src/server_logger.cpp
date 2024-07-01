#include <not_implemented.h>

#include "../include/server_logger.h"


server_logger::server_logger(std::unordered_map<server_logger::severity,
                        server_logger::logger_subscription> subscriptions, const std::string &host,
    int port):client(host, port)
{
    int process_id = getpid();

    httplib::Params params;
    params.emplace("process_id", std::to_string(process_id));
    json data;
    
    for (auto iter : subscriptions)
    {
        json inner;
        inner["console"] = iter.second._console_stream;
        inner["path"] = iter.second._paths;
        std::string severity = severity_to_string(iter.first);
        data[severity] = inner;
    }
    
    std::string message = data.dump();
    //std::cout << message;
    params.emplace("data", message);

    
    auto res = client.Post("/init", params);
    if (res && res->status != 200)
    {
        std::cerr << "Error sending data to server\n";
    }

}

server_logger::~server_logger() noexcept
{
    int process_id = getpid();

    httplib::Params params;
    params.emplace("process_id", std::to_string(process_id));

    auto res = client.Post("/delete", params);

    if (res && res->status != 200)
    {
        std::cerr << "Error sending data to server\n";
    }
}

logger const *server_logger::log(
    const std::string &text,
    logger::severity severity) const noexcept
{
    int process_id = getpid();

    httplib::Params params;
    params.emplace("process_id", std::to_string(process_id));
    params.emplace("severity", severity_to_string(severity));
    params.emplace("text", text);

    auto res = client.Post("/log", params);

    if (res && res->status != 200)
    {
        std::cerr << "Error sending data to server\n";
    }
    return this;
}

