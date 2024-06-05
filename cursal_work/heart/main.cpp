#include "../launcher/launcher.h"
#include "heart.h"

#include <iostream>
#include <string>


int main()
{
    int argc = 4;
    char* argv[] = {"", "C:\\Users\\yashelter\\Desktop\\coursework\\cmake-build-debug\\cursal_work\\server\\cw_server.exe",
                    "memory", "9030"};

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

	Heart heart(server_path, port, controller_type);

	std::thread heart_thread([&]() { heart.start(); });

	std::cout << "Press Enter to stop..." << std::endl;
	std::cin.ignore();
	heart.stop();
	heart_thread.join();

	return 0;
}