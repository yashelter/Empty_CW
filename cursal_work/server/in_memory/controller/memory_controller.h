//
// Created by Des Caldnd on 6/2/2024.
//

#ifndef MP_OS_MEMORY_CONTROLLER_H
#define MP_OS_MEMORY_CONTROLLER_H

#include "../../interfaces/controller_int.h"
#include "../tree/b_tree.h"
#include <shared_mutex>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <boost/thread/future.hpp>

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
class memory_controller : public controller_int<tkey, tvalue>
{

public:

    using collection_t = B_tree<tkey, tvalue, compare, t>;
    using shceme_t = B_tree<std::string, collection_t, std::less<std::string>, t>;
    using pool_t = B_tree<std::string, shceme_t, std::less<std::string>, t>;
    using data_t = B_tree<std::string, pool_t , std::less<std::string>, t>;


private:

    using time_point_t = controller_int<tkey, tvalue>::time_point_t;

    data_t _root;

    std::mutex _map_mut;
    std::unordered_map<CW_GUID, nlohmann::json> _request_result;

public:

    CW_GUID add_pool(std::string pool_name) override;
    CW_GUID remove_pool(std::string pool_name) override;

    CW_GUID add_scheme(std::string pool_name, std::string scheme_name) override;
    CW_GUID remove_scheme(std::string pool_name, std::string scheme_name) override;

    CW_GUID add_collection(std::string pool_name, std::string scheme_name, std::string collection_name) override;
    CW_GUID remove_collection(std::string pool_name, std::string scheme_name, std::string collection_name) override;

    CW_GUID insert(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key, tvalue value) override; // insert if not exist
    CW_GUID read_value(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key, bool need_persist, time_point_t time = std::chrono::utc_clock::now()) override;
    CW_GUID read_range(std::string pool_name, std::string scheme_name, std::string collection_name, tkey lower, tkey upper, bool need_persist, time_point_t time = std::chrono::utc_clock::now()) override;
    CW_GUID update(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key) override; // updates if exist
    CW_GUID remove(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key) override;

    std::optional<nlohmann::json> get(CW_GUID id) override;

    virtual ~memory_controller() =default;

};


#endif //MP_OS_MEMORY_CONTROLLER_H
