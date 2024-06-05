//
// Created by Des Caldnd on 6/3/2024.
//

#include "heart.h"
#include <iostream>
#include <unistd.h>

Heart::Heart(const std::string& server_path, uint16_t port, const std::string& controller_type)
		: server_path_(server_path), port_(port), controller_type_(controller_type), is_running(false)
{
	url_ = "http://127.0.0.1:" + std::to_string(port);
	server_process_ = process::launchProcess(server_path, "");
}

Heart::~Heart()
{
	stop();
}

void Heart::start()
{
	is_running = true;
	monitor_thread_ = std::thread(&::Heart::monitor_our_server, this);
}

void Heart::stop()
{
	is_running = false;
	if (monitor_thread_.joinable()) // тут не уверен правильно ли
	{
		monitor_thread_.join();
	}
	process::stopProcess(server_process_);
}

void Heart::monitor_our_server()
{
	while (is_running)
	{
		if (!check_server())
		{
			restart_server();
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

bool Heart::check_server()
{
	httplib::Client _client(url_.c_str());
	auto res = _client.Get("/heart", {}, httplib::Headers());
	if (res && res->status == 200)
	{
		return true;
	}
	return false;
}

void Heart::restart_server()
{
	std::cout << "Restarting server..." << std::endl;
	process::stopProcess(server_process_);
	server_process_ = process::launchProcess(server_path_, "");
}

