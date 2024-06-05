//
// Created by Des Caldnd on 6/2/2024.
//

#ifndef MP_OS_MEMORY_CONTROLLER_H
#define MP_OS_MEMORY_CONTROLLER_H

#include <controller_int.h>
#include "../tree/b_tree.h"
#include <shared_mutex>
#include <server_logger.h>
#include <ranges>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <boost/thread/future.hpp>

template<serializable tkey, serializable tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class disk_controller;

template<serializable tkey, serializable tvalue, compator<tkey> compare = std::less<tkey>, size_t t = 5>
class memory_controller : public controller_int<tkey, tvalue>, public logger_guardant
{

public:

    friend disk_controller<tkey, tvalue, compare, t>;

    using collection_t = B_tree<tkey, tvalue, compare, t>;
    using scheme_t = B_tree<std::string, collection_t, std::less<std::string>, t>;
    using pool_t = B_tree<std::string, scheme_t, std::less<std::string>, t>;
    using data_t = B_tree<std::string, pool_t , std::less<std::string>, t>;


private:

    using time_point_t = controller_int<tkey, tvalue>::time_point_t;

    compare _comparer;
    data_t _root;
    allocator* _allocator;
    logger* _logger;

    std::mutex _map_mut;
    std::unordered_map<CW_GUID, nlohmann::json> _request_result;

    template <typename collection>
    class perst_helper
    {
        collection& _c;

    public:

        perst_helper(collection& c, time_point_t time);

        ~perst_helper();
    };

public:

    logger *get_logger() const override;

    explicit memory_controller(compare _comparer = compare(), allocator* = nullptr, logger* = nullptr);

    CW_GUID add_pool(std::string pool_name) override;
    CW_GUID remove_pool(std::string pool_name) override;

    CW_GUID add_scheme(std::string pool_name, std::string scheme_name) override;
    CW_GUID remove_scheme(std::string pool_name, std::string scheme_name) override;

    CW_GUID add_collection(std::string pool_name, std::string scheme_name, std::string collection_name) override;
    CW_GUID remove_collection(std::string pool_name, std::string scheme_name, std::string collection_name) override;

    CW_GUID insert(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key, tvalue value) override; // insert if not exist
    CW_GUID read_value(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key, bool need_persist, time_point_t time = std::chrono::utc_clock::now()) override;
    CW_GUID read_range(std::string pool_name, std::string scheme_name, std::string collection_name, tkey lower, tkey upper, bool need_persist, time_point_t time = std::chrono::utc_clock::now()) override;
    CW_GUID update(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key, tvalue value) override; // updates if exist
    CW_GUID remove(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key) override;

    std::optional<nlohmann::json> get(CW_GUID id) override;

    ~memory_controller() override =default;

};

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::read_range(std::string pool_name, std::string scheme_name,
                                                                std::string collection_name, tkey lower, tkey upper,
                                                                bool need_persist, memory_controller::time_point_t time)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name, scheme_name, collection_name, lower, upper, need_persist, time](){

        if (need_persist)
        {
            nlohmann::json res;

            std::lock_guard lock(_root._mut);

            perst_helper<data_t> hlp(_root, time);

            auto pool_it = _root.find(pool_name);

            if (pool_it == _root.end())
            {
                res["message"] = "Pool with such name does not exist";
                return res;
            }

            std::lock_guard slock(pool_it->second._mut);

            perst_helper<pool_t> phlp(pool_it->second, time);

            auto scheme_it = pool_it->second.find(scheme_name);

            if (scheme_it == pool_it->second.end())
            {
                res["message"] = "Scheme with such name does not exists";
                return res;
            }

            std::lock_guard clock(scheme_it->second._mut);

            perst_helper<scheme_t> chlp(scheme_it->second, time);

            auto coll_it = scheme_it->second.find(collection_name);

            if (coll_it == scheme_it->second.end())
            {
                res["message"] = "Collection with such name does not exists";
                return res;
            }

            std::lock_guard dlock(coll_it->second._mut);

            perst_helper<collection_t> dhlp(coll_it->second, time);

            auto [beg, en] = coll_it->second.find_range(lower, upper);

            if (beg == en)
            {
                res["message"] = "Data with such key does not exist";
                return res;
            }

            nlohmann::json k;

            std::ranges::for_each(std::ranges::subrange(beg, en), [&k](auto pair){
                k.push_back(pair);
            }, [](auto& pair)
            {
                nlohmann::json tmp;
                tmp["key"] = pair.first.to_json();
                tmp["value"] = pair.second.to_json();

                return tmp;
            });


            res["data"] = k;

            return res;

        } else
        {
            nlohmann::json res;

            std::shared_lock lock(_root._mut);

            auto pool_it = _root.find(pool_name);

            if (pool_it == _root.end())
            {
                res["message"] = "Pool with such name does not exist";
                return res;
            }

            std::shared_lock slock(pool_it->second._mut);

            auto scheme_it = pool_it->second.find(scheme_name);

            if (scheme_it == pool_it->second.end())
            {
                res["message"] = "Scheme with such name does not exists";
                return res;
            }

            std::shared_lock clock(scheme_it->second._mut);

            auto coll_it = scheme_it->second.find(collection_name);

            if (coll_it == scheme_it->second.end())
            {
                res["message"] = "Collection with such name does not exists";
                return res;
            }

            std::shared_lock dlock(coll_it->second._mut);

            auto [beg, en] = coll_it->second.find_range(lower, upper);

            if (beg == en)
            {
                res["message"] = "Data with such key does not exist";
                return res;
            }

            nlohmann::json k;

            std::ranges::for_each(std::ranges::subrange(beg, en), [&k](auto pair){
                k.push_back(pair);
            }, [](auto& pair)
                                  {
                                      nlohmann::json tmp;
                                      tmp["key"] = pair.first.to_json();
                                      tmp["value"] = pair.second.to_json();

                                      return tmp;
                                  });


            res["data"] = k;

            return res;
        }

    }).then([this, id](auto fut){
        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, fut.get()));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
template<typename collection>
memory_controller<tkey, tvalue, compare, t>::perst_helper<collection>::~perst_helper()
{
    _c.redo_all();
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
template<typename collection>
memory_controller<tkey, tvalue, compare, t>::perst_helper<collection>::perst_helper(collection &c,
                                                                                    memory_controller::time_point_t time) : _c(c)
{
    _c.revert_to(time);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::read_value(std::string pool_name, std::string scheme_name,
                                                                std::string collection_name, tkey key,
                                                                bool need_persist, memory_controller::time_point_t time)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name, scheme_name, collection_name, key, need_persist, time](){

        if (need_persist)
        {
            nlohmann::json res;

            std::lock_guard lock(_root._mut);

            perst_helper<data_t> hlp(_root, time);

            auto pool_it = _root.find(pool_name);

            if (pool_it == _root.end())
            {
                res["message"] = "Pool with such name does not exist";
                return res;
            }

            std::lock_guard slock(pool_it->second._mut);

            perst_helper<pool_t> phlp(pool_it->second, time);

            auto scheme_it = pool_it->second.find(scheme_name);

            if (scheme_it == pool_it->second.end())
            {
                res["message"] = "Scheme with such name does not exists";
                return res;
            }

            std::lock_guard clock(scheme_it->second._mut);

            perst_helper<scheme_t> chlp(scheme_it->second, time);

            auto coll_it = scheme_it->second.find(collection_name);

            if (coll_it == scheme_it->second.end())
            {
                res["message"] = "Collection with such name does not exists";
                return res;
            }

            std::lock_guard dlock(coll_it->second._mut);

            perst_helper<collection_t> dhlp(coll_it->second, time);

            auto it = coll_it->second.find(key);

            if (it == coll_it->second.end())
            {
                res["message"] = "Data with such key does not exist";
                return res;
            }

            res["data"] = it->second.to_json();

            return res;

        } else
        {
            nlohmann::json res;

            std::shared_lock lock(_root._mut);

            auto pool_it = _root.find(pool_name);

            if (pool_it == _root.end())
            {
                res["message"] = "Pool with such name does not exist";
                return res;
            }

            std::shared_lock slock(pool_it->second._mut);

            auto scheme_it = pool_it->second.find(scheme_name);

            if (scheme_it == pool_it->second.end())
            {
                res["message"] = "Scheme with such name does not exists";
                return res;
            }

            std::shared_lock clock(scheme_it->second._mut);

            auto coll_it = scheme_it->second.find(collection_name);

            if (coll_it == scheme_it->second.end())
            {
                res["message"] = "Collection with such name does not exists";
                return res;
            }

            std::shared_lock dlock(coll_it->second._mut);

            auto it = coll_it->second.find(key);

            if (it == coll_it->second.end())
            {
                res["message"] = "Data with such key does not exist";
                return res;
            }

            res["data"] = it->second.to_json();

            return res;
        }

    }).then([this, id](auto fut){
        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, fut.get()));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::update(std::string pool_name, std::string scheme_name,
                                                            std::string collection_name, tkey key, tvalue value)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name, scheme_name, collection_name, key, value](){
        std::shared_lock lock(_root._mut);

        auto pool_it = _root.find(pool_name);

        if (pool_it == _root.end())
            return std::string("Pool with such name does not exist");

        std::shared_lock slock(pool_it->second._mut);

        auto scheme_it = pool_it->second.find(scheme_name);

        if (scheme_it == pool_it->second.end())
            return std::string("Scheme with such name does not exists");

        std::shared_lock clock(scheme_it->second._mut);

        auto coll_it = scheme_it->second.find(collection_name);

        if (coll_it == scheme_it->second.end())
            return std::string("Collection with such name does not exists");

        std::lock_guard dlock(coll_it->second._mut);

        auto it = coll_it->second.find(key);

        if (it == coll_it->second.end())
            return std::string("Data with such key does not exist");

        coll_it->second.update(std::make_pair(key, value));

        return std::string("Data updated");

    }).then([this, id](auto fut){
        nlohmann::json j;

        j["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, j));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::remove(std::string pool_name, std::string scheme_name,
                                                            std::string collection_name, tkey key)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name, scheme_name, collection_name, key](){
        std::shared_lock lock(_root._mut);

        auto pool_it = _root.find(pool_name);

        if (pool_it == _root.end())
            return std::string("Pool with such name does not exist");

        std::shared_lock slock(pool_it->second._mut);

        auto scheme_it = pool_it->second.find(scheme_name);

        if (scheme_it == pool_it->second.end())
            return std::string("Scheme with such name does not exists");

        std::shared_lock clock(scheme_it->second._mut);

        auto coll_it = scheme_it->second.find(collection_name);

        if (coll_it == scheme_it->second.end())
            return std::string("Collection with such name does not exists");

        std::lock_guard dlock(coll_it->second._mut);

        auto it = coll_it->second.find(key);

        if (it == coll_it->second.end())
            return std::string("Data with such key does not exist");

        coll_it->second.erase(it);

        return std::string("Data removed");

    }).then([this, id](auto fut){
        nlohmann::json j;

        j["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, j));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::insert(std::string pool_name, std::string scheme_name,
                                                            std::string collection_name, tkey key, tvalue value)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name, scheme_name, collection_name, key, value](){
        std::shared_lock lock(_root._mut);

        auto pool_it = _root.find(pool_name);

        if (pool_it == _root.end())
            return std::string("Pool with such name does not exist");

        std::shared_lock slock(pool_it->second._mut);

        auto scheme_it = pool_it->second.find(scheme_name);

        if (scheme_it == pool_it->second.end())
            return std::string("Scheme with such name does not exists");

        std::shared_lock clock(scheme_it->second._mut);

        auto coll_it = scheme_it->second.find(collection_name);

        if (coll_it == scheme_it->second.end())
            return std::string("Collection with such name does not exists");

        std::lock_guard dlock(coll_it->second._mut);

        auto [it, res] = coll_it->second.insert(std::make_pair(key, value));

        if (!res)
            return std::string("Data with such key already exists");

        return std::string("Data inserted");

    }).then([this, id](auto fut){
        nlohmann::json j;

        j["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, j));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
std::optional<nlohmann::json> memory_controller<tkey, tvalue, compare, t>::get(CW_GUID id)
{
    std::cout << id << std::endl;

    std::lock_guard lock(_map_mut);

    auto it = _request_result.find(id);

    if (it == _request_result.end())
        return {};
    else
    {
        auto data = it->second;
        _request_result.erase(it);
        return data;
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::remove_collection(std::string pool_name, std::string scheme_name,
                                                                       std::string collection_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name, scheme_name, collection_name](){
        std::shared_lock lock(_root._mut);

        auto pool_it = _root.find(pool_name);

        if (pool_it == _root.end())
            return std::string("Pool with such name does not exist");

        std::shared_lock slock(pool_it->second._mut);

        auto scheme_it = pool_it->second.find(scheme_name);

        if (scheme_it == pool_it->second.end())
            return std::string("Scheme with such name does not exists");

        std::lock_guard clock(scheme_it->second._mut);

        auto coll_it = scheme_it->second.find(collection_name);

        if (coll_it == scheme_it->second.end())
            return std::string("Collection with such name does not exists");

        scheme_it->second.erase(coll_it);

        return std::string("Collection removed");

    }).then([this, id](auto fut){
        nlohmann::json j;

        j["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, j));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::add_collection(std::string pool_name, std::string scheme_name,
                                                                       std::string collection_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name, scheme_name, collection_name](){
        std::shared_lock lock(_root._mut);

        auto pool_it = _root.find(pool_name);

        if (pool_it == _root.end())
            return std::string("Pool with such name does not exist");

        std::shared_lock slock(pool_it->second._mut);

        auto scheme_it = pool_it->second.find(scheme_name);

        if (scheme_it == pool_it->second.end())
            return std::string("Scheme with such name does not exists");

        std::lock_guard clock(scheme_it->second._mut);

        auto [coll_it, res] = scheme_it->second.insert(std::make_pair(collection_name, collection_t(_comparer, _allocator, _logger)));

        if (!res)
            return std::string("Collection with such name already exists");

        return std::string("Collection added");

    }).then([this, id](auto fut){
        nlohmann::json j;

        j["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, j));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::remove_scheme(std::string pool_name, std::string scheme_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name, scheme_name](){
        std::shared_lock lock(_root._mut);

        auto pool_it = _root.find(pool_name);

        if (pool_it == _root.end())
            return std::string("Pool with such name does not exist");

        std::lock_guard slock(pool_it->second._mut);

        auto scheme_it = pool_it->second.find(scheme_name);

        if (scheme_it == pool_it->second.end())
            return std::string("Scheme with such name does not exists");

        pool_it->second.erase(scheme_it);

        return std::string("Scheme removed");

    }).then([this, id](auto fut){
        nlohmann::json j;

        j["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, j));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::add_scheme(std::string pool_name, std::string scheme_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name, scheme_name](){
        std::shared_lock lock(_root._mut);

        auto pool_it = _root.find(pool_name);

        if (pool_it == _root.end())
            return std::string("Pool with such name does not exist");

        std::lock_guard slock(pool_it->second._mut);

        auto [scheme_it, res] = pool_it->second.insert(std::make_pair(scheme_name, scheme_t(std::less<std::string>(), _allocator, _logger)));

        if (!res)
            return std::string("Scheme with such name already exists");

        return std::string("New scheme added");

    }).then([this, id](auto fut){
        nlohmann::json j;

        j["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, j));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::remove_pool(std::string pool_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name](){
        std::lock_guard lock(_root._mut);

        auto it = _root.find(pool_name);

        if (it == _root.end())
            return std::string("Pool with such name does not exist");

        _root.erase(it);

        return std::string("Pool successfully removed");

    }).then([this, id](auto fut){
        nlohmann::json j;

        j["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, j));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID memory_controller<tkey, tvalue, compare, t>::add_pool(std::string pool_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, id, pool_name](){
        std::lock_guard lock(_root._mut);

        auto [it, res] = _root.insert(std::make_pair(pool_name, pool_t(std::less<std::string>(), _allocator, _logger)));

        std::string message = res ? "New pool created" : "Pool with same name already exists";

        return message;
    }).then([this, id](auto fut){
        nlohmann::json j;

        j["message"] = fut.get();

        std::cout <<  id << " " << j["message"] << std::endl;

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, j));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
memory_controller<tkey, tvalue, compare, t>::memory_controller(compare comparer, allocator *al, logger *lo) : _logger(lo), _allocator(al), _root(std::less<std::string>(), al, lo), _comparer(comparer) {}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
logger *memory_controller<tkey, tvalue, compare, t>::get_logger() const
{
    return _logger;
}


#endif //MP_OS_MEMORY_CONTROLLER_H
