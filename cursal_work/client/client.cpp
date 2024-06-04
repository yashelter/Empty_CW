#include "client.h"

const std::regex Client::_exit_reg("exit", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_hint_reg("hint", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_heart_reg("heart", std::regex_constants::icase | std::regex_constants::optimize);
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
std::optional<std::string> Client::get(const std::string &guid)
{
    httplib::Params params;
    params.emplace("GUID", guid);

    auto res = _client.Get("/get", params, httplib::Headers());

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

std::optional<std::string> Client::add_pool(const std::string &pool_name)
{
    httplib::Params params;
    params.emplace("pool_name", pool_name);

    auto res = _client.Get("/add_pool", params, httplib::Headers());

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

    auto res = _client.Get("/remove_pool", params, httplib::Headers());

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

    auto res = _client.Get("/add_scheme", params, httplib::Headers());

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

    auto res = _client.Get("/remove_scheme", params, httplib::Headers());

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

    auto res = _client.Get("/add_collection", params, httplib::Headers());

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

    auto res = _client.Get("/remove_collection", params, httplib::Headers());

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

std::optional<std::string>
Client::insert(const std::string &pool_name,
               const std::string &scheme_name,
               const std::string &collection_name,
               const json &student)
{
    httplib::Params params;

    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("data", to_string(student));

    auto res = _client.Get("/insert", params, httplib::Headers());

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }
    return {};
}

std::optional<std::string>
Client::remove(const std::string &pool_name,
               const std::string &scheme_name,
               const std::string &collection_name,
               const std::string &student)
{
    httplib::Params params;

    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("data", student);

    auto res = _client.Get("/remove", params, httplib::Headers());

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }
    return {};
}


std::optional<std::string>
Client::update(const std::string &pool_name,
               const std::string &scheme_name,
               const std::string &collection_name,
               const json &student)
{
    httplib::Params params;

    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("data", to_string(student));

    auto res = _client.Get("/update", params, httplib::Headers());

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }
    return {};
}


std::optional<std::string> Client::read_value(const std::string &pool_name,
                                              const std::string &scheme_name,
                                              const std::string &collection_name,
                                              const std::string &key,
                                              bool need_persist,
                                              std::time_t time)

{
    httplib::Params params;
    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("key", key);
    params.emplace("need_persist", std::to_string(need_persist));
    params.emplace("time_point", std::to_string(time));

    auto res = _client.Get("/read_value", params, httplib::Headers());

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}

std::optional<std::string> Client::read_range(const std::string &pool_name,
                                              const std::string &scheme_name,
                                              const std::string &collection_name,
                                              const std::string &up,
                                              const std::string &down,
                                              bool need_persist,
                                              std::time_t time)

{
    httplib::Params params;
    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("lower", up);
    params.emplace("upper", down);
    params.emplace("need_persist", std::to_string(need_persist));
    params.emplace("time_point", std::to_string(time));

    auto res = _client.Get("/read_range", params, httplib::Headers());

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
    return "";
}

std::string Client::get_hint()
{
    std::string str;
    str = "To add new pool : new_pool {pool name}\n";
    str += "hint : show available commands\n";
    str += "remove_pool {pool_name} : add new pool\n";
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
               cout << invalid_param;
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
        else if (std::regex_match(command, _read_value_reg))
        {
            std::string arg2, arg3, arg4;
            cin >> arg >> arg2 >> arg3 >> arg4;
            if (!std::regex_match(arg, _all_num_reg) || !std::regex_match(arg2, _all_num_reg) ||
                !std::regex_match(arg3, _all_num_reg) || !std::regex_match(arg4, _all_num_reg))
            {
                cout << invalid_param << std::endl;
                continue;
            }
            bool need_persist = false;
            cout << "Do you want look old data: 0-No, 1-Yes >> ";
            cin >> need_persist;
            std::time_t time = 0;

            if (need_persist)
            {
                std::string line;
                do
                {
                    cout << "Enter date in format %d.%m.%Y-%H:%M:%S> (numbers with symbols) >>  ";
                    cin >> line;
                    time = input_time(line);
                } while (time == 0);
            }

            std::optional<std::string> response = read_value(arg, arg2, arg3, arg4,
                                                             need_persist,
                                                             need_persist ? time :std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

            if (!response.has_value())
            {
                cout << std::endl;
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
        else if (std::regex_match(command, _heart_reg))
        {
            bool is_alive = heart();
            if (is_alive)
            {
                cout << "Server working successfully\n";
            }
            else
            {
                cout << "Server not response\n";
            }
        }
        else if (std::regex_match(command, _hint_reg))
        {
            cout << get_hint() << std::endl;
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

student Client::read_student(std::istream &cin, std::ostream &cout)
{
    student stud;
    do
    {
        cout << "Enter student surname >> ";
        cin >> stud._surname;
        cout << std::endl;
    } while (!std::regex_match(stud._surname, _all_num_reg));

    do
    {
        cout << "Enter student name >> ";
        cin >> stud._name;
        cout << std::endl;
    } while (!std::regex_match(stud._surname, _all_num_reg));

    do
    {
        cout << "Enter student group >> ";
        cin >> stud._group;
        cout << std::endl;
    } while (!std::regex_match(stud._surname, _all_num_reg));

    cout << "Enter student course >> ";
    cin >> stud._course;
    cout << std::endl;

    cout << "Enter pairs subject - mark,  <end>- to end entering marks" <<
        std::endl << "Enter subject >> ";

    std::string subj;
    unsigned short mark;
    while (cin >> subj && subj != "end")
    {
        cout << "Enter mark :" << std::endl;
        cin >> mark;
        stud._subjects.emplace_back(subj, mark);
        cout << "Enter subject >> ";
    }

    cout << "Student successfully made\n";
    return stud;
}

bool Client::heart()
{
    // httplib::Params params;
    auto res = _client.Get("/heart", {}, httplib::Headers());
    return (res && res->status == 200);
}

std::string Client::get_new_student(std::istream &cin, std::ostream &cout)
{
    student stud = read_student(cin, cout);
    json js = stud.to_json();
    return to_string(js);
}


std::time_t Client::input_time(const std::string& line)
{
    std::tm t = {};
    std::istringstream ss(line);
    //ss.imbue(std::locale("de_DE.utf-8"));

    ss >> std::get_time(&t, "%d.%m.%Y-%H:%M:%S");

    if (ss.fail())
    {
        return 0;
    }

    std::time_t time = std::mktime(&t);
    return time;
}