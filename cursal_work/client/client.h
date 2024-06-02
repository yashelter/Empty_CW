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


public:

    Client(const std::string& destination);

    void start_dialog(std::ifstream& stream);
};


#endif //MP_OS_CLIENT_H
