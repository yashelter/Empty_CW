//
// Created by Des Caldnd on 6/3/2024.
//

#ifndef MP_OS_HEART_H
#define MP_OS_HEART_H

#include "../client/client.h"
#include "../launcher/launcher.h"
#include <httplib.h>

class Heart
{
public:

	Heart(const std::string& server_path, uint16_t port, const std::string& controller_type);

	~Heart();

	void start();

	void stop();

private:

	void monitor_our_server();

	bool check_server();

	void restart_server();

	std::string server_path_;

	uint16_t port_;

	std::string controller_type_;

	std::string url_;

	std::thread monitor_thread_;

	bool is_running;

	process::process_handle_t server_process_;
};


#endif //MP_OS_HEART_H
