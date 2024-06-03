#include "client.h"


const std::regex Client::_exit_reg("exit", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_mode_reg("change_mode", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_add_pool_reg("new_pool", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_remove_pool_reg("remove_pool", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_add_scheme_reg("add_scheme ", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_remove_scheme_reg("remove_scheme ", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_add_collection_reg("add_collection", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_remove_collection_reg("remove_collection  ", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_insert_reg("insert", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_read_value_reg("read_value", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_read_range_reg("read_range", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_update_reg("update", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_all_num_reg("^[A-Za-z0-9]+$", std::regex_constants::optimize);

Client::Client(const std::string& destination) : _client(destination)
{

}

std::optional<std::string> Client::add_pool(const std::string &pool_name)
{
    httplib::Params params;
    params.emplace("pool_name", pool_name);

    auto res = _client.Post("/add_pool", params);

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

std::optional<std::string> Client::get(const std::string &guid)
{
    httplib::Params params;
    params.emplace("CW_GUID", guid);

    auto res = _client.Post("/get", params);

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

std::optional<std::string> Client::remove_pool(const std::string &pool_name)
{
    httplib::Params params;
    params.emplace("pool_name", pool_name);

    auto res = _client.Post("/remove_pool", params);

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

std::optional<std::string> Client::add_scheme(const std::string &pool_name, const std::string &scheme_name)
{
    httplib::Params params;
    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);

    auto res = _client.Post("/add_scheme", params);

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}


std::optional<std::string> Client::remove_scheme(const std::string &pool_name, const std::string &scheme_name)
{
    httplib::Params params;
    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);

    auto res = _client.Post("/remove_scheme", params);

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

std::optional<std::string> Client::add_collection(const std::string &pool_name,
                                                  const std::string &scheme_name,
                                                  const std::string &collection_name)
{
    httplib::Params params;
    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);

    auto res = _client.Post("/add_collection", params);

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

std::optional<std::string> Client::remove_collection(const std::string &pool_name,
                                                  const std::string &scheme_name,
                                                  const std::string &collection_name)
{
    httplib::Params params;
    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);

    auto res = _client.Post("/remove_collection", params);

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

// TODO: 7-10

std::optional<std::string> Client::read_value(const std::string &pool_name,
                                              const std::string &scheme_name,
                                              const std::string &collection_name,
                                              bool need_persist,
                                              Client::time_point_t time_point)

{
    httplib::Params params;
    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("need_persist", std::to_string(need_persist)); // ?
    //TODO: params.emplace("time_point", time_point); // ?

    auto res = _client.Post("/read_value", params);

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

std::optional<std::string> Client::read_range(const std::string &pool_name,
                                              const std::string &scheme_name,
                                              const std::string &collection_name,
                                              bool need_persist,
                                              Client::time_point_t time_point)

{
    httplib::Params params;
    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("need_persist", std::to_string(need_persist)); // ?
    //TODO: params.emplace("time_point", time_point); // ?

    auto res = _client.Post("/read_range", params);

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}


std::string Client::get_answer_from_server(const std::string& guid)
{
    if (response_strategy == ResponseStrategy::Waiting)
    {
        auto answer = get(guid);

        while (!answer.has_value())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            answer = get(guid);
        }
        return answer.value();
        // TODO: parse answer
    } else if (response_strategy == ResponseStrategy::Async)
    {
        auto future = std::async(std::launch::async, [this, guid]()
        {
            std::optional<std::string> answer;
            while (!answer.has_value())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                answer = get(guid);
            }
            return answer.value();
            // TODO: parse answer
        });
        return future.get();
    }
}

std::string Client::get_hint()
{
    std::string str;
    str = "To add new pool : new_pool {pool name}\n";
    str += "hint : show available commands\n";
    str += "To exit : exit\n";
    return str;
}

// /read_value - pool_name scheme_name collection_name need_persist:bool (time_point)
// body: json(key:string)
void Client::start_dialog(std::istream &cin, std::ostream &cout)
{
    std::string str = "Successfully started client\nAwaiting your commands\n\n";
    cout << str << get_hint();

    std::string command, arg;

    while (cin >> command)
    {
        if (std::regex_match(command, _add_pool_reg))
        {
           cin >> arg;
           if (!std::regex_match(arg, _all_num_reg))
           {
               cout << invalid_param << std::endl;
               continue;
           }
           std::optional<std::string> response = add_pool(arg);
           if (!response.has_value())
           {
               cout << "Failed while sent message\n";
               continue;
           }
           cout << "Getted Answer :" << get_answer_from_server(response.value()) << std::endl;
           // TODO: parse answer
        }
        else if (std::regex_match(command, _remove_pool_reg))
        {
            cin >> arg;
            if (!std::regex_match(arg, _all_num_reg))
            {
                cout << invalid_param << std::endl;
                continue;
            }
            std::optional<std::string> response = remove_pool(arg);
            if (!response.has_value())
            {
                cout << "Failed while sent message\n";
                continue;
            }
            cout << "Getted Answer :" << get_answer_from_server(response.value()) << std::endl;
            // TODO: parse answer
        }
        else if (std::regex_match(command, _add_scheme_reg))
        {
            std::string arg2;
            cin >> arg >> arg2;
            if (!std::regex_match(arg, _all_num_reg) || !std::regex_match(arg2, _all_num_reg))
            {
                cout << invalid_param << std::endl;
                continue;
            }
            std::optional<std::string> response = add_scheme(arg, arg2);
            if (!response.has_value())
            {
                cout << "Failed while sent message\n";
                continue;
            }
            cout << "Getted Answer :" << get_answer_from_server(response.value()) << std::endl;
            // TODO: parse answer
        }
        else if (std::regex_match(command, _remove_scheme_reg))
        {
            std::string arg2;
            cin >> arg >> arg2;
            if (!std::regex_match(arg, _all_num_reg) || !std::regex_match(arg2, _all_num_reg))
            {
                cout << invalid_param << std::endl;
                continue;
            }
            std::optional<std::string> response = remove_scheme(arg, arg2);
            if (!response.has_value())
            {
                cout << "Failed while sent message\n";
                continue;
            }
            cout << "Getted Answer :" << get_answer_from_server(response.value()) << std::endl;
            // TODO: parse answer
        }
        else if (std::regex_match(command, _add_collection_reg))
        {
            std::string arg2, arg3;
            cin >> arg >> arg2 >> arg3;
            if (!std::regex_match(arg, _all_num_reg) || !std::regex_match(arg2, _all_num_reg) || !std::regex_match(arg3, _all_num_reg))
            {
                cout << invalid_param << std::endl;
                continue;
            }
            std::optional<std::string> response = add_collection(arg, arg2, arg3);
            if (!response.has_value())
            {
                cout << "Failed while sent message\n";
                continue;
            }
            cout << "Getted Answer :" << get_answer_from_server(response.value()) << std::endl;
            // TODO: parse answer
        }
        else if (std::regex_match(command, _remove_collection_reg))
        {
            std::string arg2, arg3;
            cin >> arg >> arg2 >> arg3;
            if (!std::regex_match(arg, _all_num_reg) || !std::regex_match(arg2, _all_num_reg) || !std::regex_match(arg3, _all_num_reg))
            {
                cout << invalid_param << std::endl;
                continue;
            }
            std::optional<std::string> response = remove_collection(arg, arg2, arg3);
            if (!response.has_value())
            {
                cout << "Failed while sent message\n";
                continue;
            }
            cout << "Getted Answer :" << get_answer_from_server(response.value()) << std::endl;
            // TODO: parse answer
        }
        // TODO: read value, read range, update
        else if (std::regex_match(command, _exit_reg))
        {
            cout << "Stopping Client App" << std::endl;
            return;
        }
        else if (std::regex_match(command, _mode_reg))
        {
            // Can add dialog here - but no reason
            response_strategy = response_strategy == Async ? Waiting : Async;
        }
        else
        {
            cout << "Invalid command : {" << command << "}" << std::endl;
        }
    }
}


