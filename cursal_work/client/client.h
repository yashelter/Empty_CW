#ifndef MP_OS_CLIENT_H
#define MP_OS_CLIENT_H

#include "../structure/student.h"
#include <regex>
#include <httplib.h>
#include <string>
#include <istream>
#include <chrono>
#include <thread>
#include <future>

using json = nlohmann::json;

class Client
{
public:
    enum ResponseStrategy
    {
        Async, Waiting
    };
    ResponseStrategy response_strategy;
private:
    mutable httplib::Client _client;

    static const std::regex _add_pool_reg;
    static const std::regex _hint_reg;
    static const std::regex _exit_reg;
    static const std::regex _heart_reg;
	static const std::regex _all_num_reg;
	static const std::regex _mode_reg;
	static const std::regex _remove_pool_reg;
	static const std::regex _add_scheme_reg;
	static const std::regex _remove_scheme_reg;
	static const std::regex _add_collection_reg;
	static const std::regex _remove_collection_reg;
	static const std::regex _insert_reg;
	static const std::regex _read_value_reg;
	static const std::regex _read_range_reg;
	static const std::regex _update_reg;
    /*
     * pools, schemas, collections name can contain only A-Z a-z 0-9 _ -
     * Нужно подумать о типе, которые будем хранить
     * Например ключ - строка - фамилия студента, а значение - структура - Имя, курс, группа, вектор пар из названия предмета и оценки
     * При нашей архитектуре сделать такое несложно, но зато можно выебнуться
     *
     * Также предлагаю сделать 2 режима: с ожиданием и без
     *
     * С ожиданием после запроса каждые 100мс будем посылать get запросы и если не вышло писать что-то типа waiting..., а потом результат
     * Без ожидания делаем как раз с future с продолжением, чтобы когда получил значение вывел автоматически
     *
     * Почитай про basic_syncbuf для этого
     */
private:
    static student read_student(std::istream& cin, std::ostream& cout);
    static std::time_t input_time(const std::string& line);

    std::optional<std::string> add_pool(const std::string& pool_name);
    std::optional<std::string> remove_pool(const std::string& pool_name);

    std::optional<std::string> add_scheme(const std::string &pool_name, const std::string &scheme_name);
    std::optional<std::string> remove_scheme(const std::string &pool_name, const std::string &scheme_name);

    std::optional<std::string> add_collection(const std::string &pool_name, const std::string &scheme_name, const std::string &collection_name);
    std::optional<std::string> remove_collection(const std::string &pool_name, const std::string &scheme_name, const std::string &collection_name);

    std::optional<std::string> insert(const std::string& pool_name,
                                          const std::string& scheme_name,
                                          const std::string& collection_name,
                                          const json &student);

    std::optional<std::string> update(const std::string& pool_name,
                                      const std::string& scheme_name,
                                      const std::string& collection_name,
                                      const json &student);

    std::optional<std::string> remove(const std::string& pool_name,
                                      const std::string& scheme_name,
                                      const std::string& collection_name,
                                      const std::string &student);

    std::optional<std::string> read_value(const std::string& pool_name,
                                          const std::string& scheme_name,
                                          const std::string& collection_name,
                                          const std::string& key,
                                          bool need_persist = false,
                                          std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

    std::optional<std::string> read_range(const std::string& pool_name,
                                          const std::string& scheme_name,
                                          const std::string& collection_name,
                                          const std::string& key1,
                                          const std::string& key2,
                                          bool need_persist = false,
                                          std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

    const std::string invalid_param = "Got invalid parameters, please check them to rules\n";
    std::string get_hint();

    std::optional<std::string> get(const std::string &guid);
    void get_answer_from_server(std::istream& cin, std::ostream& cout, const std::string& guid,
                                void (*function)(std::ostream&, std::string&));
    static std::string get_new_student(std::istream& cin, std::ostream& cout);
    static void simple_parse(std::ostream& cout, std::string& line);

public:

    explicit Client(const std::string& destination="http://127.0.0.1:9300");

    void start_dialog(std::istream& cin = std::cin, std::ostream& cout = std::cout);
    bool heart();

};


#endif //MP_OS_CLIENT_H
