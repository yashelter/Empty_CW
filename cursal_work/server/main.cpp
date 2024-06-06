//
// Created by Des Caldnd on 6/2/2024.
//
#include <iostream>

#include <thread>
#include <chrono>

#include <vector>

#include <memory_controller.h>
#include "../structure/student.h"

#include "server.h"

#include <allocator_sorted_list.h>
#include <allocator_boundary_tags.h>
#include <allocator_global_heap.h>
#include <allocator_buddies_system.h>

#include <server_logger_builder.h>
#include <server_logger.h>

using namespace std::chrono_literals;

int main()
{
    int argc = 3;
    char* argv[] = {"", "memory", "9030"};

	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <controller_type> <port>" << std::endl;
		return 1;
	}

	char* controller_type = argv[1];

	uint16_t port;
	try
	{
		port = std::stoul(argv[2]);
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << "invalid port number: " << argv[2] << std::endl;
		return 1;
	}
	catch (const std::out_of_range& e)
	{
		std::cerr << "port number out of range: " << argv[2] << std::endl;
		return 1;
	}

	server_logger_builder builder;
	controller_int<cw_string, student>* controller;
	auto allocator = std::make_unique<allocator_global_heap>(nullptr);
	std::unique_ptr<logger> logger(builder.build());
	auto comparer = std::less<cw_string>();

	if (strcmp(controller_type, "memory") == 0)
	{
		controller = new memory_controller<cw_string, student>(comparer, allocator.get(), logger.get());
	}
	else if (strcmp(controller_type, "disk") == 0)
	{
		//controller = new disk_controller<cw_string, student, std::less<cw_string>, 1>("database", logger.get());
	}
	else
	{
		std::cerr << "invalid controller type: " << controller_type << std::endl;
		return 1;
	}

	Server<cw_string, student> server(controller, port);

	return 0;
}


