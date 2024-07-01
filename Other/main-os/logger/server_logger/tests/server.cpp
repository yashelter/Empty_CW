#include <iostream>
#include "httplib.h"
#include <unordered_map>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <vector>

#include "../include/server_logger.h"

httplib::Server svr;
std::unordered_map<int, std::unordered_map<logger::severity, std::pair<std::vector<std::string>, bool>>> datas;
std::mutex mutex;

void parsing_severity(int procces_id, logger::severity severity, json& js)
{
    if (js.empty() || !js.is_object())
    {
        return;
    }
    auto &data_map = datas[procces_id];

    auto answ = data_map.emplace(severity, make_pair(std::vector<std::string>(), false));
    if (!answ.second)
    {
        throw std::ios_base::failure("problem with emplace in hash_map\n");
    }
    auto &_map = data_map[severity];

    auto console_iter = js.find("console");
    if (console_iter != js.end() && console_iter->is_boolean() && console_iter->get<bool>())
    {
        _map.second = true;
    }

    auto path_iter = js.find("path");
    if (path_iter != js.end() && path_iter->is_array())
    {
        nlohmann::json arr = *path_iter;
        for (const nlohmann::json& obj : arr)
        {
            if (!obj.is_string() || obj.empty())
            {
                continue;
            }
            std::string path_file = obj.get<std::string>();
            _map.first.push_back(path_file);
        }
    }


}

void make_log(const httplib::Request& req, httplib::Response& res)
{
    std::lock_guard<std::mutex> lock(mutex); // cамо выходит и даже при исключении

    int process_id = std::atoi(req.get_param_value("process_id").c_str());
    std::string sev_str = req.get_param_value("severity");
    logger::severity severity = logger::string_to_severity(sev_str);
    std::string mes = req.get_param_value("text");

    auto iter = datas.find(process_id);
    if (iter == datas.end())
    {
        return;
    }
    auto &_map = iter->second;

    auto iterl = _map.find(severity);
    if (iterl == _map.end())
    {
        return;
    }
    auto& vals = iterl->second;
    if (vals.second == true)
    {
        std::cout << "[" << sev_str << "] " << mes << std::endl;
    }
    for (std::string path : vals.first)
    {
        std::ofstream file(path, std::ios::out | std::ios::app);

        if (file.is_open())
        {
            file << mes << "\n";
            file.close();
        }
        else
        {
            std::cerr << "Unable to open file for writing.\n";
        }
    }

}

void make_delete(const httplib::Request& req, httplib::Response& res)
{
    std::lock_guard<std::mutex> lock(mutex);
    int process_id = std::atoi(req.get_param_value("process_id").c_str());

    auto iter = datas.find(process_id);
    if (iter == datas.end())
    {
        return;
    }
    datas.erase(process_id);
}

void initializate(const httplib::Request& req, httplib::Response& res)
{
    // Извлечение данных из тела POST-запроса
    // freeze
    std::lock_guard<std::mutex> lock(mutex);
    int process_id = std::atoi(req.get_param_value("process_id").c_str());

    auto result = datas.emplace(process_id, std::unordered_map<logger::severity, std::pair<std::vector<std::string>, bool>>());
    if (!result.second)
    {
        throw std::ios_base::failure("problem with emplace in hash_map\n");
    }

    std::string str = req.get_param_value("data");
    //std::cout << str; debug


    json jsonData = json::parse(str);

    parsing_severity(process_id, logger::severity::information, jsonData["INFORMATION"]);
    parsing_severity(process_id, logger::severity::warning, jsonData["WARNING"]);
    parsing_severity(process_id, logger::severity::error, jsonData["ERROR"]);
    parsing_severity(process_id, logger::severity::critical, jsonData["CRITICAL"]);
    parsing_severity(process_id, logger::severity::debug, jsonData["DEBUG"]);
    parsing_severity(process_id, logger::severity::trace, jsonData["TRACE"]);
    

    res.set_content("Data received", "text/plain");
}


int main(
    int argc,
    char* argv[])
{
    // Создание HTTP-сервера
    httplib::Server svr;

    // Обработка POST-запросов по указанному пути
    svr.Post("/init", initializate);
    svr.Post("/log", make_log);
    svr.Post("/delete", make_delete);


    std::cout << "Server started at http://localhost:8080" << std::endl;

    svr.listen("localhost", 8080);
}
