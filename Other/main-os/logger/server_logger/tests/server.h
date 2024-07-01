#include "httplib.h"
#include "../include/server_logger.h"
#include <unordered_map>
#include <mutex>

class server
{
	httplib::Server svr;
private:
	static std::unordered_map<int, server_logger::logger_subscription> _all_streams;
public:
	void initializate(const httplib::Request&, httplib::Response&);
	server();
	~server();
}