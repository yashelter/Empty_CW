#include "heart.h"
#include <iostream>
#include <unistd.h>
#include <atomic>
#include <stop_token>
#include <httplib.h>

Heart::Heart(const std::string& server_path, uint16_t port, const std::string& controller_type)
		: server_path_(server_path), port_(port), controller_type_(controller_type), is_running(false),
		  client_("http://127.0.0.1:" + std::to_string(port))
{
	url_ = "http://127.0.0.1:" + std::to_string(port);
	server_process_ = process::launchProcess(server_path, controller_type + " " + std::to_string(port));
}

Heart::~Heart()
{
	stop();
}

void Heart::start()
{
	is_running = true;
	while (is_running)
	{
		if (!check_server())
		{
			restart_server();
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void Heart::stop()
{
	is_running = false;
	process::stopProcess(server_process_);
	std::cout << "Stopping server process";
}

bool Heart::check_server()
{
	return client_.heart();
}

void Heart::restart_server()
{
	std::cout << "Restarting server..." << std::endl;
	process::stopProcess(server_process_);
	server_process_ = process::launchProcess(server_path_, controller_type_ + " " + std::to_string(port_));
}