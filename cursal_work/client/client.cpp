#include "client.h"

const std::regex Client::_exit_reg("exit", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_hint_reg("hint", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_heart_reg("heart", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_mode_reg("change_mode", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_add_pool_reg("new_pool", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_remove_pool_reg("remove_pool", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_add_scheme_reg("new_scheme", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_remove_scheme_reg("remove_scheme", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_add_collection_reg("new_collection", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_remove_collection_reg("remove_collection", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_insert_reg("insert", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_remove_reg("remove", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_read_value_reg("read_value", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_read_range_reg("read_range", std::regex_constants::icase | std::regex_constants::optimize);
const std::regex Client::_update_reg("update", std::regex_constants::icase | std::regex_constants::optimize);

const std::regex Client::_all_num_reg("^[A-Za-z]+[A-Za-z0-9]*$", std::regex_constants::optimize);

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
        std::cout << res->body << std::endl;
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
               const std::string &student)
{
    httplib::Params params;

    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("data", student);

    httplib::Headers headers;

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
               const std::string &surname)
{
    httplib::Params params;

    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("data", surname);

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
               const std::string &student)
{
    httplib::Params params;

    params.emplace("pool_name", pool_name);
    params.emplace("scheme_name", scheme_name);
    params.emplace("collection_name", collection_name);
    params.emplace("data", student);

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
    params.emplace("time", std::to_string(time));

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
    params.emplace("time", std::to_string(time));

    auto res = _client.Get("/read_range", params, httplib::Headers());

    if (res && res->status == 200)
    {
        return std::optional<std::string> {res->body};
    }

    return {};
}


void Client::get_answer_from_server(std::istream& cin, std::ostream& cout, const std::string& guid,
                                    void (*function)(std::ostream&, std::string&))
{
    std::cout << guid << std::endl;
    if (response_strategy == ResponseStrategy::Waiting)
    {
        auto answer = get(guid);

        while (!answer.has_value())
        {
            cout << "Waiting for response...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            answer = get(guid);
        }
        function(cout, answer.value());
    } else if (response_strategy == ResponseStrategy::Async)
    {
        auto future = std::async(std::launch::async, [this, guid, &cout, function]()
        {
            std::optional<std::string> answer;
            while (!answer.has_value())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                answer = get(guid);
            }
            function(cout, answer.value());
        });

    }

}


std::string Client::get_hint()
{
    std::string str;
    str = "hint : shows available commands\n";
    str += "new_pool {pool name} : add new pool\n";
    str += "remove_pool {pool_name} : removes existing pool\n";

    str += "new_scheme {pool name} {scheme_name} : add new scheme\n";
    str += "remove_scheme {pool name} {scheme_name} : removes existing scheme\n";

    str += "new_collection {pool name} {scheme_name} {collection name} : add new collection\n";
    str += "remove_collection {pool name} {scheme_name} {collection name} : removes existing collection\n";

    str += "insert  {pool name} {scheme_name} {collection name} {student (input)} : inserts new student\n";
    str += "update {pool name} {scheme_name} {collection name} {student (input), surname=key} : update student with this surname\n";
    str += "remove {pool name} {scheme_name} {collection name} {surname=key} : removes existing student with given surname\n";
    str += "read_value {pool name} {scheme_name} {collection name} {surname=key} : returns student with given surname\n";
    str += "read_range {pool name} {scheme_name} {collection name} {surname=up_key}  {surname=down_key} : returns students in range with given surname\n";

    str += "exit : stops dialog\n";
    return str;
}

void Client::start_dialog(std::istream &cin, std::ostream &cout)
{
    std::string str = "Successfully started client\nAwaiting your commands\n\n";
    cout << str << get_hint();

    while (true)
    {
        std::string command, arg;
        cout << "Enter command : ";
        cin >> command;
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
           get_answer_from_server(cin, cout, response.value(), &parse_message);
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
            get_answer_from_server(cin, cout, response.value(),&parse_message);
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
            get_answer_from_server(cin, cout, response.value(),&parse_message);
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
            get_answer_from_server(cin, cout, response.value(),&parse_message);
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
            get_answer_from_server(cin, cout, response.value(),&parse_message);
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
            get_answer_from_server(cin, cout, response.value(),&simple_parse);
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

                    std::chrono::time_point<std::chrono::system_clock> x = (std::chrono::system_clock::from_time_t(time));
                    std::cout << time << " " << (std::chrono::system_clock::from_time_t(time) < std::chrono::system_clock::now()) << " ";
                    std::cout << x << std::endl;

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
            get_answer_from_server(cin, cout, response.value(),&parse_student);
        }
        else if (std::regex_match(command, _read_range_reg))
        {
            std::string arg2, arg3, arg4, arg5;
            cin >> arg >> arg2 >> arg3 >> arg4 >> arg5;
            if (!std::regex_match(arg, _all_num_reg) || !std::regex_match(arg2, _all_num_reg) ||
                !std::regex_match(arg3, _all_num_reg) || !std::regex_match(arg4, _all_num_reg) || !std::regex_match(arg5, _all_num_reg))
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

            std::optional<std::string> response = read_range(arg, arg2, arg3, arg4,arg5,
                                                             need_persist,
                                                             need_persist ? time :std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

            if (!response.has_value())
            {
                cout << std::endl;
                cout << "Failed while sent message\n";
                continue;
            }
            get_answer_from_server(cin, cout, response.value(),&simple_parse);
        }
        else if (std::regex_match(command, _update_reg))
        {
            std::string arg2, arg3;
            cin >> arg >> arg2 >> arg3;
            if (!std::regex_match(arg, _all_num_reg) || !std::regex_match(arg2, _all_num_reg) ||
                !std::regex_match(arg3, _all_num_reg))
            {
                cout << invalid_param << std::endl;
                continue;
            }
            std::string stud = get_new_student(cin, cout);

            std::optional<std::string> response = update(arg, arg2, arg3, stud);

            if (!response.has_value())
            {
                cout << std::endl;
                cout << "Failed while sent message\n";
                continue;
            }
            get_answer_from_server(cin, cout, response.value(),&parse_message);
        }
        else if (std::regex_match(command, _remove_reg))
        {
            std::string arg2, arg3, surname;
            cin >> arg >> arg2 >> arg3 >> surname;
            if (!std::regex_match(arg, _all_num_reg) || !std::regex_match(arg2, _all_num_reg) ||
                !std::regex_match(arg3, _all_num_reg) || !std::regex_match(surname, _all_num_reg))
            {
                cout << invalid_param << std::endl;
                continue;
            }

            std::optional<std::string> response = remove(arg, arg2, arg3, surname);

            if (!response.has_value())
            {
                cout << std::endl;
                cout << "Failed while sent message\n";
                continue;
            }
            get_answer_from_server(cin, cout, response.value(),&parse_message);
        }
        else if (std::regex_match(command, _insert_reg))
        {
            std::string arg2, arg3;
            cin >> arg >> arg2 >> arg3;
            if (!std::regex_match(arg, _all_num_reg) || !std::regex_match(arg2, _all_num_reg) ||
                !std::regex_match(arg3, _all_num_reg))
            {
                cout << invalid_param << std::endl;
                continue;
            }
            std::string res = get_new_student(cin, cout);
            //student st = student::from_json(json::parse(res));
            //cout << st.to_json().dump() << std::endl;

            std::optional<std::string> response = insert(arg, arg2, arg3, res);

            if (!response.has_value())
            {
                cout << std::endl;
                cout << "Failed while sent message\n";
                continue;
            }
            get_answer_from_server(cin, cout, response.value(),&parse_message);
        }
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
                cout << std::endl << "Server not working...\n";
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
    } while (!std::regex_match(stud._surname, _all_num_reg));

    do
    {
        cout << "Enter student name >> ";
        cin >> stud._name;
    } while (!std::regex_match(stud._surname, _all_num_reg));

    do
    {
        cout << "Enter student group >> ";
        cin >> stud._group;
    } while (!std::regex_match(stud._surname, _all_num_reg));

    cout << "Enter student course >> ";
    cin >> stud._course;
    cout << "Enter pairs subject - mark,  <end>- to end entering marks" << std::endl;

    std::string subj;

    while (subj != "end")
    {
        cout << "Enter subject : ";

        std::string temp;
        cin >> temp;

        if (temp != "end")
        {
            unsigned short mark;
            cout << "Enter mark : ";
            cin >> mark;
            stud._subjects.emplace_back(temp, mark);
        }
        subj = temp;
    }

    cout << "Student successfully made\n";
    return stud;
}

bool Client::heart()
{
    httplib::Params params;
    auto res = _client.Get("/heart", params, httplib::Headers());
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

    ss >> std::get_time(&t, "%d.%m.%Y-%H:%M:%S");

    if (ss.fail())
    {
        return 0;
    }

    std::time_t time = std::mktime(&t);
    return time;
}

void Client::simple_parse(std::ostream &cout, std::string &line)
{
    json js = json::parse(line);
    cout <<  std::endl << std::setw(4) << js<< std::endl;
}

void Client::parse_message(std::ostream &cout, std::string &line)
{
    json js = json::parse(line);
    try
    {
        std::string str = js["message"].template get<std::string>();;
        cout << std::endl << str << std::endl;
    }catch (const std::exception &e)
    {
        simple_parse(cout, line);
    }

}

void Client::parse_student(std::ostream &cout, std::string &line)
{
    json js = json::parse(line);
    try
    {
        json data = js["data"]; // read_value pool1 scheme1 col1 Skywalker 0
        student stud = student::from_json(data);

        cout << std::endl << stud.to_string() << std::endl;
    }catch (const std::exception &e)
    {
        parse_message(cout, line);
    }
}

