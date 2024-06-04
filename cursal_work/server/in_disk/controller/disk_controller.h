//
// Created by Des Caldnd on 6/2/2024.
//

#ifndef MP_OS_DISK_CONTROLLER_H
#define MP_OS_DISK_CONTROLLER_H


#include <controller_int.h>
#include <operation.h>
#include "../tree/b_tree_disk.h"
#include <memory_controller.h>
#include <shared_mutex>
#include <unordered_map>
#include <condition_variable>
#include <atomic>
#include <filesystem>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <boost/thread/future.hpp>

generator<unsigned int> atomic_index_gen();


template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
class disk_controller : public controller_int<tkey, tvalue>
{

public:

    static disk_controller from_memory(const memory_controller<tkey, tvalue, compare, t>& cont, const std::string& path);
    memory_controller<tkey, tvalue, compare, t> to_memory(allocator* = nullptr) const;


private:

    class in_disk_operation : public operation<disk_controller<tkey, tvalue, compare, t>>{};

    class in_disk_pool_add_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;

        in_disk_pool_add_operation(unsigned int index, std::string pool_name);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_pool_remove_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;

        in_disk_pool_remove_operation(unsigned int index, std::string pool_name);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_scheme_add_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;


        in_disk_scheme_add_operation(unsigned int index, std::string pool_name, std::string scheme_name);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_scheme_remove_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;

        in_disk_scheme_remove_operation(unsigned int index, std::string pool_name, std::string scheme_name);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_collection_add_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;
        std::string _collection_name;

        in_disk_collection_add_operation(unsigned int index, std::string pool_name, std::string scheme_name, std::string collection_name);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_collection_remove_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;
        std::string _collection_name;

        in_disk_collection_remove_operation(unsigned int index, std::string pool_name, std::string scheme_name, std::string collection_name);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_data_insert_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;
        std::string _collection_name;
        std::pair<tkey, tvalue> _data;

        in_disk_data_insert_operation(unsigned int index, std::string pool_name, std::string scheme_name, std::string collection_name, std::pair<tkey, tvalue> data);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_data_update_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;
        std::string _collection_name;
        std::pair<tkey, tvalue> _data;

        in_disk_data_update_operation(unsigned int index, std::string pool_name, std::string scheme_name, std::string collection_name, std::pair<tkey, tvalue> data);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_pool_data_remove_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;
        std::string _collection_name;
        std::pair<tkey, tvalue> _data;

        in_disk_pool_data_remove_operation(unsigned int index, std::string pool_name, std::string scheme_name, std::string collection_name, std::pair<tkey, tvalue> data);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    using time_point_t = controller_int<tkey, tvalue>::time_point_t;

    std::filesystem::path _root;
    std::filesystem::path _bucket;
    logger* _logger;

    std::mutex _map_mut;
    std::unordered_map<CW_GUID, nlohmann::json> _request_result;

    std::mutex _history_mut;
    std::stack<std::pair<time_point_t, std::shared_ptr<in_disk_operation>>> _history;
    std::stack<std::pair<time_point_t, std::shared_ptr<in_disk_operation>>> _undone_history;

    std::unordered_map<std::string, std::pair<bool, size_t>> _opened_dirs_files;
    std::mutex _opened_mutex;
    std::condition_variable _opened_cond_var;

    generator<unsigned int> _index_gen = atomic_index_gen();

    class lock_helper
    {
        std::unordered_map<std::string, std::pair<bool, size_t>>& _map;
        bool _unique;
        std::string& _path;
        std::mutex& _mut;
        std::condition_variable& _cond_var;

        lock_helper(std::unordered_map<std::string, std::pair<bool, size_t>>& map, bool unique, std::string& path, std::mutex& mut, std::condition_variable& cond_var);
        ~lock_helper() noexcept;
    };

public:

    explicit disk_controller(const std::string& path = "database", compare comparer = compare(), logger* logger = nullptr);

    disk_controller(const disk_controller& other) =delete;
    disk_controller(disk_controller&& other) noexcept =default;

    disk_controller& operator=(const disk_controller& other) =delete;
    disk_controller& operator=(disk_controller&& other) noexcept =default;

    static void remove_dir(std::filesystem::path path, std::filesystem::path to, unsigned int ind);

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

    virtual ~disk_controller() noexcept =default;

};

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::insert(std::string pool_name, std::string scheme_name,
                                                          std::string collection_name, tkey key, tvalue value)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name, collection_name](){
        lock_helper lock(_opened_dirs_files, false, "", _opened_mutex, _opened_cond_var);

        bool res = std::filesystem::exists(_root / pool_name);

        if (!res)
        {
            return std::string("Pool does not exist");
        }

        lock_helper plock(_opened_dirs_files, false, pool_name, _opened_mutex, _opened_cond_var);

        res = std::filesystem::exists(_root / pool_name / scheme_name);

        if (!res)
        {
            return std::string("Scheme does not exist");
        }

        lock_helper slock(_opened_dirs_files, false, pool_name + '/' + scheme_name, _opened_mutex, _opened_cond_var);


        res = std::filesystem::exists(_root / pool_name / scheme_name / (collection_name + ".dat"));

        if (!res)
            return std::string("Collection does not exist");

        lock_helper clock(_opened_dirs_files, true, pool_name + '/' + scheme_name + '/' + collection_name, _opened_mutex, _opened_cond_var);

        B_tree_disk<tkey, tvalue, compare, t> tree(_root / pool_name / scheme_name / (collection_name + ".dat"), _logger);

        if (!tree.is_valid())
        {
            return std::string("Collection was not opened");


        }
        _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(new in_disk_collection_remove_operation(ind, pool_name, scheme_name, collection_name))));
        return std::string("Collection was successfully removed");

    }).then([this, id](auto fut){
        nlohmann::json res;
        res["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, res));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::remove_collection(std::string pool_name, std::string scheme_name,
                                                                     std::string collection_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name, collection_name](){
        lock_helper lock(_opened_dirs_files, false, "", _opened_mutex, _opened_cond_var);

        bool res = std::filesystem::exists(_root / pool_name);

        if (!res)
        {
            return std::string("Pool does not exist");
        }

        lock_helper plock(_opened_dirs_files, false, pool_name, _opened_mutex, _opened_cond_var);

        res = std::filesystem::exists(_root / pool_name / scheme_name);

        if (!res)
        {
            return std::string("Scheme does not exist");
        }

        lock_helper slock(_opened_dirs_files, true, pool_name + '/' + scheme_name, _opened_mutex, _opened_cond_var);


        res = std::filesystem::exists(_root / pool_name / scheme_name / (collection_name + ".dat"));


        if (res)
        {
            std::lock_guard hlock(_history_mut);

            size_t length = 0;

            unsigned int ind = _index_gen();

            remove_dir(_root / pool_name / scheme_name / (collection_name + ".dat"), _bucket, ind);

            _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(new in_disk_collection_remove_operation(ind, pool_name, scheme_name, collection_name))));
            return std::string("Collection was successfully removed");
        } else
        {
            return std::string("Collection was not removed");
        }

    }).then([this, id](auto fut){
        nlohmann::json res;
        res["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, res));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::add_collection(std::string pool_name, std::string scheme_name,
                                                                  std::string collection_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name, collection_name](){
        lock_helper lock(_opened_dirs_files, false, "", _opened_mutex, _opened_cond_var);

        bool res = std::filesystem::exists(_root / pool_name);

        if (!res)
        {
            return std::string("Pool does not exist");
        }

        lock_helper plock(_opened_dirs_files, false, pool_name, _opened_mutex, _opened_cond_var);

        res = std::filesystem::exists(_root / pool_name / scheme_name);

        if (!res)
        {
            return std::string("Scheme does not exist");
        }

        lock_helper slock(_opened_dirs_files, true, pool_name + '/' + scheme_name, _opened_mutex, _opened_cond_var);



        std::fstream file;

        if (!std::filesystem::exists(_root / pool_name / scheme_name / (collection_name + ".dat")))
            file.open(_root / pool_name / scheme_name / (collection_name + ".dat"), std::ios_base::binary);

        res = file.is_open();

        if (res)
        {
            std::lock_guard hlock(_history_mut);

            size_t length = 0;

            file.write(reinterpret_cast<char*>(length), sizeof(size_t));
            file.write(reinterpret_cast<char*>(length), sizeof(size_t));

            _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(new in_disk_collection_add_operation(_index_gen(), pool_name, scheme_name, collection_name))));
            return std::string("Collection was successfully created");
        } else
        {
            return std::string("Collection was not created");
        }

    }).then([this, id](auto fut){
        nlohmann::json res;
        res["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, res));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::remove_scheme(std::string pool_name, std::string scheme_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name](){
        lock_helper lock(_opened_dirs_files, false, "", _opened_mutex, _opened_cond_var);

        bool res = std::filesystem::exists(_root / pool_name);

        if (!res)
        {
            return std::string("Pool does not exist");
        }

        lock_helper plock(_opened_dirs_files, true, pool_name, _opened_mutex, _opened_cond_var);

        res = std::filesystem::exists(_root / pool_name / scheme_name);

        if (res)
        {
            std::lock_guard hlock(_history_mut);

            auto ind = _index_gen();

            remove_dir(_root / pool_name / scheme_name, _bucket, ind);

            _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(new in_disk_scheme_remove_operation(ind, pool_name, scheme_name))));
            return std::string("Scheme was successfully removed");
        } else
        {
            return std::string("Scheme was not removed");
        }

    }).then([this, id](auto fut){
        nlohmann::json res;
        res["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, res));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::add_scheme(std::string pool_name, std::string scheme_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name](){
        lock_helper lock(_opened_dirs_files, false, "", _opened_mutex, _opened_cond_var);

        bool res = std::filesystem::exists(_root / pool_name);

        if (!res)
        {
            return std::string("Pool does not exist");
        }

        lock_helper plock(_opened_dirs_files, true, pool_name, _opened_mutex, _opened_cond_var);

        res = std::filesystem::create_directory(_root / pool_name / scheme_name);

        if (res)
        {
            std::lock_guard hlock(_history_mut);
            _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(new in_disk_scheme_add_operation(_index_gen(), pool_name, scheme_name))));
            return std::string("Scheme was successfully created");
        } else
        {
            return std::string("Scheme was not created");
        }

    }).then([this, id](auto fut){
        nlohmann::json res;
        res["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, res));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
std::optional<nlohmann::json> disk_controller<tkey, tvalue, compare, t>::get(CW_GUID id)
{
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
CW_GUID disk_controller<tkey, tvalue, compare, t>::remove_pool(std::string pool_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name](){
        lock_helper lock(_opened_dirs_files, true, "", _opened_mutex, _opened_cond_var);

        bool res = std::filesystem::exists(_root / pool_name);

        if (res)
        {
            std::lock_guard hlock(_history_mut);

            auto ind = _index_gen();

            remove_dir(_root / pool_name, _bucket, ind);

            _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(new in_disk_pool_remove_operation(ind, pool_name))));
            return std::string("Pool was successfully removed");
        } else
        {
            return std::string("Pool was not removed");
        }

    }).then([this, id](auto fut){
        nlohmann::json res;
        res["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, res));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::add_pool(std::string pool_name)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name](){
        lock_helper lock(_opened_dirs_files, true, "", _opened_mutex, _opened_cond_var);

        bool res = std::filesystem::create_directory(_root / pool_name);

        if (res)
        {
            std::lock_guard hlock(_history_mut);
            _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(new in_disk_pool_add_operation(_index_gen(), pool_name))));
            return std::string("Pool was successfully created");
        } else
        {
            return std::string("Pool was not created");
        }

    }).then([this, id](auto fut){
        nlohmann::json res;
        res["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, res));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::disk_controller(const std::string &path, logger *logger) : _logger(logger)
{
    _root = std::filesystem::weakly_canonical("./" + path);
    _bucket = std::filesystem::weakly_canonical("./" + path + "_bucket");

    std::filesystem::remove_all(_bucket);
}


#endif //MP_OS_DISK_CONTROLLER_H
