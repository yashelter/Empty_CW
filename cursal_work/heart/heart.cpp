// Created by Des Caldnd on 6/3/2024.
//

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
	server_process_ = process::launchProcess(server_path, std::to_string(port) + " " + controller_type);
}

Heart::~Heart()
{
	stop();
}

void Heart::start()
{
	is_running = true;
	monitor_thread_ = std::jthread(&Heart::monitor_our_server, this);
}

void Heart::stop()
{
	is_running = false;
	if (monitor_thread_.joinable()) {
		monitor_thread_.request_stop();
		monitor_thread_.join();
	}
	process::stopProcess(server_process_);
	std::cout << "Stopping server process: " << server_process_ << std::endl;
}

void Heart::monitor_our_server(std::stop_token stoken)
{
	while (!stoken.stop_requested() && is_running)
	{
		if (!check_server()) {
			restart_server();
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

bool Heart::check_server()
{
	auto res = client_.Get("/heart", {}, httplib::Headers());
	return res && res->status == 200;
}

void Heart::restart_server()
{
	std::cout << "Restarting server..." << std::endl;
	process::stopProcess(server_process_);
	server_process_ = process::launchProcess(server_path_, std::to_string(port_) + " " + controller_type_);
}
