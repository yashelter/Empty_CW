#include "server.h"

using json = nlohmann::json;


template <
	serializable tkey,
	serializable tvalue>
Server<tkey, tvalue>::Server(controller_int<tkey, tvalue>* controller, uint16_t port)
		: _controller(controller)
{
	CROW_ROUTE(app, "/add_pool") ([&](const crow::request &req)
    {
		auto pool_name = req.url_params.get("pool_name");
		_controller->add_pool(pool_name);

		return 0; // Так?
	});

	CROW_ROUTE(app, "/remove_pool") ([&](const crow::request &req) {
		auto pool_name = req.url_params.get("pool_name");
		_controller->remove_pool(pool_name);

		return 0; // Так?
	});

	CROW_ROUTE(app, "/add_scheme") ([&](const crow::request &req) {
		auto pool_name = req.url_params.get("pool_name");
		auto scheme_name = req.url_params.get("scheme_name");
		_controller->add_scheme(pool_name, scheme_name);

		return 0; // Так?
	});

	CROW_ROUTE(app, "/remove_scheme") ([&](const crow::request &req) {
		auto pool_name = req.url_params.get("pool_name");
		auto scheme_name = req.url_params.get("scheme_name");
		_controller->add_scheme(pool_name, scheme_name);

	   return 0; // Так?
   });

	CROW_ROUTE(app, "/add_collection") ([&](const crow::request &req) {
		auto pool_name = req.url_params.get("pool_name");
		auto scheme_name = req.url_params.get("scheme_name");
		auto collection_name = req.url_params.get("collection_name");
		_controller->add_collection(pool_name, scheme_name, collection_name);

		return 0; // Так?
	});

	CROW_ROUTE(app, "/remove_collection") ([&](const crow::request &req) {
		auto pool_name = req.url_params.get("pool_name");
		auto scheme_name = req.url_params.get("scheme_name");
		auto collection_name = req.url_params.get("collection_name");

		_controller->remove_collection(pool_name, scheme_name, collection_name);

		return 0; // Так?
	});

	CROW_ROUTE(app, "/insert") ([&](const crow::request &req)
	{
		auto pool_name = req.url_params.get("pool_name");
		auto scheme_name = req.url_params.get("scheme_name");
		auto collection_name = req.url_params.get("collection_name");

		auto data_json = req.url_params.get("data");

		json data = json::parse(data_json);
		if (data.find("key") != data.end() && data.find("value") != data.end())
		{
			tkey key = data["key"];
			tvalue value = data["value"];

			_controller->insert(pool_name, scheme_name, collection_name, key, value);

			return crow::response(200, "OK");
		}

		return crow::response(400, "'data' parameter is missing or empty");
	});


	CROW_ROUTE(app, "/read_value") ([&](const crow::request &req)
	{
		auto pool_name = req.url_params.get("pool_name");
		auto scheme_name = req.url_params.get("scheme_name");
		auto collection_name = req.url_params.get("collection_name");
		auto need_persist = req.url_params.get("need_persist");
		auto time_str = req.url_params.get("time");

		typename controller_int<tkey, tvalue>::time_point_t time;
		if (!time_str){
			time = std::chrono::system_clock::from_time_t(std::stoll(time_str)); // TODO
		} else {
			time = std::chrono::system_clock::now();
		}

		auto data_json = req.url_params.get("data");

		json data = json::parse(data_json);
		if (data.find("key") != data.end() && data.find("value") != data.end())
		{
			tkey key = data["key"];
			tvalue value = data["value"];

			CW_GUID result = _controller->read_value(pool_name, scheme_name, collection_name, key, need_persist, time);

			return crow::response(200, "OK"); // TODO
		}

		return crow::response(400, "'data' parameter is missing or empty");
	});

	CROW_ROUTE(app, "/read_range") ([&](const crow::request &req) {
		auto pool_name = req.url_params.get("pool_name");
		auto scheme_name = req.url_params.get("scheme_name");
		auto collection_name = req.url_params.get("collection_name");
		auto need_persist = req.url_params.get("need_persist");
		auto time_str = req.url_params.get("time");

		typename controller_int<tkey, tvalue>::time_point_t time;
		if (!time_str){
			time = std::chrono::system_clock::from_time_t(std::stoll(time_str)); // TODO
		} else {
			time = std::chrono::system_clock::now();
		}

		auto data_json = req.url_params.get("data");
		json data = json::parse(data_json);
		if (data.find("lower") != data.end() && data.find("upper") != data.end())
		{
			tkey lower_key = data["lower"];
			tkey upper_key =  data["upper"];

			CW_GUID result = _controller->read_range(pool_name, scheme_name, collection_name, lower_key, upper_key, need_persist, time);

			return crow::response(200, "OK"); // TODO
		}

		return crow::response(400, "'data' parameter is missing or empty");
	});

	CROW_ROUTE(app, "/update") ([&](const crow::request &req) {
		auto pool_name = req.url_params.get("pool_name");
		auto scheme_name = req.url_params.get("scheme_name");
		auto collection_name = req.url_params.get("collection_name");

		auto data_json = req.url_params.get("data");
		json data = json::parse(data_json);
		if (data.find("key") != data.end())
		{

			tkey key = data["key"];
			CW_GUID result = _controller->update(pool_name, scheme_name, collection_name, key);

			return crow::response(200, "OK"); // TODO
		}

		return crow::response(400, "'data' parameter is missing or empty");
	});

	CROW_ROUTE(app, "/remove") ([&](const crow::request &req) {
		auto pool_name = req.url_params.get("pool_name");
		auto scheme_name = req.url_params.get("scheme_name");
		auto collection_name = req.url_params.get("collection_name");

		auto data_json = req.url_params.get("data");
		json data = json::parse(data_json);
		if (data.find("key") != data.end())
		{
			tkey key = data["key"];
			CW_GUID result = _controller->remove(pool_name, scheme_name, collection_name, key);

			return crow::response(200, "OK"); // TODO
		}

		return crow::response(400, "'data' parameter is missing or empty");
	});

	CROW_ROUTE(app, "/get") ([&](const crow::request &req) {
		auto body = req.url_params.get("data");
		json data = json::parse(body);
		if (data.find("id") != data.end())
		{
			return crow::response(400, "Missing 'id' in JSON");
		}

		CW_GUID id = data["id"].get<CW_GUID>(); // TODO

		std::optional<nlohmann::json> result = _controller->get(id);

		if (result) {
			return crow::response(200, result->dump());
		} else {
			return crow::response(404, "Not Found");
		}
	});




}
