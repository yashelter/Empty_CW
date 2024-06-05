//
// Created by Des Caldnd on 6/3/2024.
//
#include "../launcher/launcher.h"
#include "heart.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <filesystem>

#include "../server/in_memory/controller/memory_controller.h"
#include "../structure/student.h"
#include "server_logger_builder.h"
#include "../server/interfaces/controller_int.h"
#include "../../allocator/allocator_global_heap/include/allocator_global_heap.h"

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <server_path> <controller_type> <port>" << std::endl;
		return 1;
	}

	char* server_path = argv[1];
	char* controller_type = argv[2];

	uint16_t port;
	try
	{
		port = std::stoul(argv[3]);
	}
	catch (const std::invalid_argument& e)
	{
		std::cerr << "Invalid port number: " << argv[3] << std::endl;
		return 1;
	}
	catch (const std::out_of_range& e)
	{
		std::cerr << "Port number out of range: " << argv[3] << std::endl;
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
		controller = new disk_controller<cw_string, student>("database", logger.get());
	}
	else
	{
		std::cerr << "Invalid controller type: " << controller_type << std::endl;
		return 1;
	}

	Heart heart(server_path, port, controller_type);
	heart.start();
	// Как остановить (закончить работу)
	heart.stop();
	return 0;
}
