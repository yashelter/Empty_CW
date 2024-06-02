//
// Created by Des Caldnd on 6/2/2024.
//

#ifndef MP_OS_SERVER_H
#define MP_OS_SERVER_H

#include <utility>
#include <crow.h>
#include "../in_memory/controller/memory_controller.h"
#include "../in_disk//controller/disk_controller.h"

template <serializable tkey, serializable tvalue>
class Server
{
    std::unique_ptr<controller_int<tkey, tvalue>> _controller;

    crow::SimpleApp app;

public:

    Server(controller_int<tkey, tvalue>* controller, uint16_t port = 9300);
};


#endif //MP_OS_SERVER_H
