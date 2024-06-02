//
// Created by Des Caldnd on 6/2/2024.
//

#ifndef MP_OS_CLIENT_H
#define MP_OS_CLIENT_H

#include <regex>
#include <httplib.h>
#include <string>
#include <istream>


class Client
{
    mutable httplib::Client _client;

    static const std::regex _exit_reg;

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

public:

    Client(const std::string& destination);

    void start_dialog(std::ifstream& stream);
};


#endif //MP_OS_CLIENT_H
