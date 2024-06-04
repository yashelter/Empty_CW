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

class Client
{
public:
    enum ResponseStrategy
    {
        Async, Waiting
    };
    ResponseStrategy response_strategy;
private:
    using time_point_t = std::chrono::time_point<std::chrono::utc_clock>;

    mutable httplib::Client _client;

    static const std::regex _add_pool_reg;
    static const std::regex _exit_reg;
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
    bool is_name_correct(const std::string& name);

    std::optional<std::string> add_pool(const std::string& pool_name);
    std::optional<std::string> remove_pool(const std::string& pool_name);

    std::optional<std::string> add_scheme(const std::string &pool_name, const std::string &scheme_name);
    std::optional<std::string> remove_scheme(const std::string &pool_name, const std::string &scheme_name);

    std::optional<std::string> add_collection(const std::string &pool_name, const std::string &scheme_name, const std::string &collection_name);
    std::optional<std::string> remove_collection(const std::string &pool_name, const std::string &scheme_name, const std::string &collection_name);

    std::optional<std::string> read_value(const std::string& pool_name,
                                          const std::string& scheme_name,
                                          const std::string& collection_name,
                                          bool need_persist = false,
                                          time_point_t time_point = std::chrono::utc_clock::now());

    std::optional<std::string> read_range(const std::string& pool_name,
                                          const std::string& scheme_name,
                                          const std::string& collection_name,
                                          bool need_persist = false,
                                          time_point_t time_point = std::chrono::utc_clock::now());

    const std::string invalid_param = "Got invalid parameters, please check them to rules";
    std::string get_hint();

    std::optional<std::string> get(const std::string &guid);
    std::string get_answer_from_server(const std::string& guid);

public:

    explicit Client(const std::string& destination="http://127.0.0.1:9300");

    void start_dialog(std::istream& cin = std::cin, std::ostream& cout = std::cout);

};


#endif //MP_OS_CLIENT_H
