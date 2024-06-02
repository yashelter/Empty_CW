//
// Created by Des Caldnd on 6/2/2024.
//

#ifndef MP_OS_DISK_CONTROLLER_H
#define MP_OS_DISK_CONTROLLER_H


#include "../../interfaces/controller_int.h"
#include "../../interfaces/operation.h"
#include "../tree/b_tree_disk.h"
#include <shared_mutex>
#include <unordered_map>
#include <condition_variable>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <boost/thread/future.hpp>

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
class disk_controller : public controller_int<tkey, tvalue>
{

public:



private:

    class in_disk_operation : public operation<disk_controller<tkey, tvalue, compare, t>>{};

    using time_point_t = controller_int<tkey, tvalue>::time_point_t;

    std::string _root;

    std::mutex _map_mut;
    std::unordered_map<CW_GUID, nlohmann::json> _request_result;

    std::shared_mutex _history_mut;
    std::stack<std::pair<time_point_t, std::shared_ptr<in_disk_operation>>> _history;
    std::stack<std::pair<time_point_t, std::shared_ptr<in_disk_operation>>> _undone_history;

    std::unordered_map<std::string, std::pair<bool, size_t>> _opened_dirs_files;
    std::mutex _opened_mutex;
    std::condition_variable _opened_cond_var;

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

    virtual ~disk_controller() =default;

};


#endif //MP_OS_DISK_CONTROLLER_H
