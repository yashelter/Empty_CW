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

        in_disk_pool_add_operation(unsigned int index, const std::string& pool_name);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_pool_remove_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;

        in_disk_pool_remove_operation(unsigned int index, const std::string& pool_name);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_scheme_add_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;


        in_disk_scheme_add_operation(unsigned int index, const std::string& pool_name, const std::string& scheme_name);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_scheme_remove_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;

        in_disk_scheme_remove_operation(unsigned int index, const std::string& pool_name, const std::string& scheme_name);

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

        in_disk_collection_add_operation(unsigned int index, const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name);

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

        in_disk_collection_remove_operation(unsigned int index, const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name);

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

        in_disk_data_insert_operation(unsigned int index, const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name, std::pair<tkey, tvalue> data);

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

        in_disk_data_update_operation(unsigned int index, const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name, std::pair<tkey, tvalue> data);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    class in_disk_data_remove_operation : public in_disk_operation
    {
    public:
        unsigned int _index;
        std::string _pool_name;
        std::string _scheme_name;
        std::string _collection_name;
        std::pair<tkey, tvalue> _data;

        in_disk_data_remove_operation(unsigned int index, const std::string& pool_name, const std::string& scheme_name, const std::string& collection_name, std::pair<tkey, tvalue> data);

        void undo(disk_controller<tkey, tvalue, compare, t>& cont) override;
        void redo(disk_controller<tkey, tvalue, compare, t>& cont) override;
    };

    friend class in_disk_operation;

    using time_point_t = controller_int<tkey, tvalue>::time_point_t;

    compare _comparer;
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

private:

    static void remove_dir(std::filesystem::path path, std::filesystem::path to, unsigned int ind);

    void revert_to(time_point_t time);
    void redo_all();

    class revert_helper
    {
        disk_controller& _d;
        time_point_t _time;

        revert_helper(disk_controller* d, time_point_t time);
        ~revert_helper();
    };

    friend class revert_helper;
};

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
memory_controller<tkey, tvalue, compare, t> disk_controller<tkey, tvalue, compare, t>::to_memory(allocator *al) const
{
    memory_controller<tkey, tvalue, compare, t> mcont(_comparer, al, _logger);

    lock_helper lock(_opened_dirs_files, false, _root, _opened_mutex, _opened_cond_var);
    for(auto& pool : std::filesystem::directory_iterator(_root))
    {
        CW_GUID res = mcont.add_pool(pool.path().filename());

        std::optional<nlohmann::json> var = mcont.get(res);

        while(!var)
        {
            var = mcont.get(res);
        }

        lock_helper plock(_opened_dirs_files, false, pool.path(), _opened_mutex, _opened_cond_var);
        for(auto& scheme : std::filesystem::directory_iterator(pool.path()))
        {
            res = mcont.add_scheme(pool.path().filename(), scheme.path().filename());

            var = mcont.get(res);

            while(!var)
            {
                var = mcont.get(res);
            }

            lock_helper slock(_opened_dirs_files, false, scheme.path(), _opened_mutex, _opened_cond_var);
            for(auto& collection : std::filesystem::directory_iterator(scheme.path()))
            {
                if (collection.path().extension() != ".dat")
                    continue;

                res = mcont.add_collection(pool.path().filename(), scheme.path().filename(), collection.path().filename().stem());

                var = mcont.get(res);

                while(!var)
                {
                    var = mcont.get(res);
                }

                lock_helper clock(_opened_dirs_files, false, scheme.path() / collection.path().stem(), _opened_mutex, _opened_cond_var);
                B_tree_disk<tkey, tvalue, compare,t> tree(collection.path(), _logger);

                for(auto& data : tree)
                {
                    res = mcont.insert(pool.path().filename(), scheme.path().filename(), collection.path().filename().stem(), data.first, data.second);

                    var = mcont.get(res);

                    while(!var)
                    {
                        var = mcont.get(res);
                    }
                }
            }
        }
    }

    return mcont;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>
disk_controller<tkey, tvalue, compare, t>::from_memory(const memory_controller<tkey, tvalue, compare, t> &cont,
                                                       const std::string &path)
{
    disk_controller<tkey, tvalue, compare, t> dcont(path, cont._comparer, cont._logger);

    std::shared_lock lock(cont._root._mut);
    for(auto& pool : cont._root)
    {
        CW_GUID res = dcont.add_pool(pool->first);

        std::optional<nlohmann::json> var = dcont.get(res);

        while(!var)
        {
            var = dcont.get(res);
        }

        std::shared_lock plock(pool->second._mut);
        for(auto& scheme : pool->second)
        {
            res = dcont.add_scheme(pool->first, scheme->first);

            var = dcont.get(res);

            while(!var)
            {
                var = dcont.get(res);
            }

            std::shared_lock slock(scheme->second._mut);
            for(auto& collection : scheme->second)
            {
                res = dcont.add_collection(pool->first, scheme->first, collection->first);

                var = dcont.get(res);

                while(!var)
                {
                    var = dcont.get(res);
                }

                std::shared_lock clock(collection->second._mut);
                for(auto& data : collection->second)
                {
                    res = dcont.insert(pool->first, scheme->first, collection->first, data->first, data->second);

                    var = dcont.get(res);

                    while(!var)
                    {
                        var = dcont.get(res);
                    }
                }
            }
        }
    }

    return dcont;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::in_disk_scheme_remove_operation::in_disk_scheme_remove_operation(
        unsigned int index, const std::string &pool_name, const std::string &scheme_name) : _index(index), _pool_name(pool_name), _scheme_name(scheme_name) {}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::in_disk_collection_add_operation::in_disk_collection_add_operation(
        unsigned int index, const std::string &pool_name, const std::string &scheme_name,
        const std::string &collection_name) : _index(index), _pool_name(pool_name), _scheme_name(scheme_name), _collection_name(collection_name) {}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::in_disk_collection_remove_operation::in_disk_collection_remove_operation(
        unsigned int index, const std::string &pool_name, const std::string &scheme_name,
        const std::string &collection_name) : _index(index), _pool_name(pool_name), _scheme_name(scheme_name), _collection_name(collection_name) {}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::in_disk_data_insert_operation::in_disk_data_insert_operation(
        unsigned int index, const std::string &pool_name, const std::string &scheme_name,
        const std::string &collection_name, std::pair<tkey, tvalue> data) : _index(index), _pool_name(pool_name), _scheme_name(scheme_name), _collection_name(collection_name), _data(data) {}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::in_disk_data_update_operation::in_disk_data_update_operation(
        unsigned int index, const std::string &pool_name, const std::string &scheme_name,
        const std::string &collection_name, std::pair<tkey, tvalue> data) : _index(index), _pool_name(pool_name), _scheme_name(scheme_name), _collection_name(collection_name), _data(data) {}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::in_disk_data_remove_operation::in_disk_data_remove_operation(
        unsigned int index, const std::string &pool_name, const std::string &scheme_name,
        const std::string &collection_name, std::pair<tkey, tvalue> data) : _index(index), _pool_name(pool_name), _scheme_name(scheme_name), _collection_name(collection_name), _data(data) {}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_data_insert_operation::undo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    B_tree_disk<tkey, tvalue, compare, t> tree(cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"), cont._comparer, cont._logger);

    tree.erase(_data);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_data_insert_operation::redo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    B_tree_disk<tkey, tvalue, compare, t> tree(cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"), cont._comparer, cont._logger);

    tree.insert(_data);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_data_update_operation::undo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    B_tree_disk<tkey, tvalue, compare, t> tree(cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"), cont._comparer, cont._logger);

    auto val = tree.at(_data.first);

    tree.update(_data);

    _data.second = val.value();
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_data_update_operation::redo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    B_tree_disk<tkey, tvalue, compare, t> tree(cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"), cont._comparer, cont._logger);

    auto val = tree.at(_data.first);

    tree.update(_data);

    _data.second = val.value();
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_data_remove_operation::undo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    B_tree_disk<tkey, tvalue, compare, t> tree(cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"), cont._comparer, cont._logger);

    tree.insert(_data);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_data_remove_operation::redo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    B_tree_disk<tkey, tvalue, compare, t> tree(cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"), cont._comparer, cont._logger);

    tree.erase(_data);
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_collection_add_operation::undo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    disk_controller<tkey, tvalue, compare, t>::remove_dir(cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"), cont._bucket, _index);
    disk_controller<tkey, tvalue, compare, t>::remove_dir(cont._root / _pool_name / _scheme_name / (_collection_name + "_data.bin"), cont._bucket, _index);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_collection_add_operation::redo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    std::filesystem::rename(cont._bucket / (std::to_string(_index) + _collection_name + ".dat"), cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"));
    std::filesystem::rename(cont._bucket / (std::to_string(_index) + _collection_name + "_data.bin"), cont._root / _pool_name / _scheme_name / (_collection_name + "_data.bin"));
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_collection_remove_operation::undo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    std::filesystem::rename(cont._bucket / (std::to_string(_index) + _collection_name + ".dat"), cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"));
    std::filesystem::rename(cont._bucket / (std::to_string(_index) + _collection_name + "_data.bin"), cont._root / _pool_name / _scheme_name / (_collection_name + "_data.bin"));
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_collection_remove_operation::redo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    disk_controller<tkey, tvalue, compare, t>::remove_dir(cont._root / _pool_name / _scheme_name / (_collection_name + ".dat"), cont._bucket, _index);
    disk_controller<tkey, tvalue, compare, t>::remove_dir(cont._root / _pool_name / _scheme_name / (_collection_name + "_data.bin"), cont._bucket, _index);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_scheme_remove_operation::undo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    std::filesystem::rename(cont._bucket / (std::to_string(_index) + _scheme_name), cont._root / _pool_name / _scheme_name);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_scheme_remove_operation::redo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    disk_controller<tkey, tvalue, compare, t>::remove_dir(cont._root / _pool_name / _scheme_name, cont._bucket, _index);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_scheme_add_operation::undo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    disk_controller<tkey, tvalue, compare, t>::remove_dir(cont._root / _pool_name / _scheme_name, cont._bucket, _index);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_scheme_add_operation::redo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    std::filesystem::rename(cont._bucket / (std::to_string(_index) + _scheme_name), cont._root / _pool_name / _scheme_name);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_pool_remove_operation::undo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    std::filesystem::rename(cont._bucket / (std::to_string(_index) + _pool_name), cont._root / _pool_name);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_pool_remove_operation::redo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    disk_controller<tkey, tvalue, compare, t>::remove_dir(cont._root / _pool_name, cont._bucket, _index);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_pool_add_operation::redo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    std::filesystem::rename(cont._bucket / (std::to_string(_index) + _pool_name), cont._root / _pool_name);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::in_disk_pool_add_operation::undo(
        disk_controller<tkey, tvalue, compare, t> &cont)
{
    disk_controller<tkey, tvalue, compare, t>::remove_dir(cont._root / _pool_name, cont._bucket, _index);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::in_disk_scheme_add_operation::in_disk_scheme_add_operation(
        unsigned int index, const std::string &pool_name, const std::string &scheme_name) : _index(index), _pool_name(pool_name), _scheme_name(scheme_name) {}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::in_disk_pool_remove_operation::in_disk_pool_remove_operation(
        unsigned int index, const std::string &pool_name) : _index(index), _pool_name(pool_name) {}


template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::in_disk_pool_add_operation::in_disk_pool_add_operation(unsigned int index,
                                                                                                  const std::string& pool_name) : _index(index), _pool_name(pool_name) {}


template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::lock_helper::~lock_helper() noexcept
{
    {
        std::lock_guard lock(_mut);

        auto it = _map.find(_path);

        --it->second.second;

        if (it->second.second == 0)
            _map.erase(it);
    }
    _cond_var.notify_all();
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::lock_helper::lock_helper(
        std::unordered_map<std::string, std::pair<bool, size_t>> &map, bool unique, std::string &path, std::mutex &mut,
        std::condition_variable &cond_var) : _map(map), _unique(unique), _path(path), _mut(mut), _cond_var(cond_var)
{
    std::unique_lock lock(mut);

    cond_var.wait(lock, [this](){
        auto it = _map.find(_path);

        if (it == _map.end())
            return true;

        if (!_unique && !it->second.first)
            return true;

        return false;
    });

    auto it = _map.find(_path);

    if (it == _map.end())
    {
        _map.insert(std::make_pair(path, std::make_pair(_unique, 1)));
    } else
    {
        ++it->second.second;
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::remove_dir(std::filesystem::path path, std::filesystem::path to,
                                                           unsigned int ind)
{
    auto name = path.filename();

    std::filesystem::rename(path, to / (std::to_string(ind) + name.string()));
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::revert_helper::~revert_helper()
{
    _d.redo_all();
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
disk_controller<tkey, tvalue, compare, t>::revert_helper::revert_helper(disk_controller *d,
                                                                        disk_controller::time_point_t time) : _d(d), _time(time)
{
    _d.revert_to(time);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::redo_all()
{
    std::lock_guard lock(_history_mut);

    while(!_undone_history.empty)
    {
        _undone_history.top().second->redo();
        _history.push(_undone_history.top());
        _undone_history.pop();
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
void disk_controller<tkey, tvalue, compare, t>::revert_to(disk_controller::time_point_t time)
{
    std::lock_guard lock(_history_mut);

    while(!_history.empty && _history.top().first > time)
    {
        _history.top().second->undo();
        _undone_history.push(_history.top());
        _history.pop();
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::read_range(std::string pool_name, std::string scheme_name,
                                                              std::string collection_name, tkey lower, tkey upper,
                                                              bool need_persist, disk_controller::time_point_t time)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name, collection_name, lower, upper, need_persist, time](){

        if (need_persist)
        {
            lock_helper lock(_opened_dirs_files, true, "", _opened_mutex, _opened_cond_var);

            revert_helper rev(*this, time);

            if(!std::filesystem::exists(_root / pool_name))
            {
                nlohmann::json res;
                res["message"] = "Pool does not exist";
                return res;
            }

            if(!std::filesystem::exists(_root / pool_name / scheme_name))
            {
                nlohmann::json res;
                res["message"] = "Scheme does not exist";
                return res;
            }

            if(!std::filesystem::exists(_root / pool_name / scheme_name / (collection_name + ".dat")))
            {
                nlohmann::json res;
                res["message"] = "Collection does not exist";
                return res;
            }

            B_tree_disk<tkey, tvalue, compare, t> tree(_root / pool_name / scheme_name / (collection_name + ".dat"), _logger);

            if(!tree.is_valid())
            {
                nlohmann::json res;
                res["message"] = "Collection could not be opened";
                return res;
            }

            auto val = tree.find_range(lower, upper);

            nlohmann::json res, k;

            for(auto& pair : val)
            {
                nlohmann::json tmp;
                tmp["key"] = pair.first;
                tmp["value"] = pair.second;

                k.push_back(tmp);
            }

            res["data"] = k;

            return res;
        } else
        {
            lock_helper lock(_opened_dirs_files, false, "", _opened_mutex, _opened_cond_var);

            if(!std::filesystem::exists(_root / pool_name))
            {
                nlohmann::json res;
                res["message"] = "Pool does not exist";
                return res;
            }

            lock_helper plock(_opened_dirs_files, false, pool_name, _opened_mutex, _opened_cond_var);

            if(!std::filesystem::exists(_root / pool_name / scheme_name))
            {
                nlohmann::json res;
                res["message"] = "Scheme does not exist";
                return res;
            }

            lock_helper slock(_opened_dirs_files, false, pool_name + '/' + scheme_name, _opened_mutex, _opened_cond_var);

            if(!std::filesystem::exists(_root / pool_name / scheme_name / (collection_name + ".dat")))
            {
                nlohmann::json res;
                res["message"] = "Collection does not exist";
                return res;
            }

            lock_helper clock(_opened_dirs_files, false, pool_name + '/' + scheme_name + '/' + collection_name, _opened_mutex, _opened_cond_var);

            B_tree_disk<tkey, tvalue, compare, t> tree(_root / pool_name / scheme_name / (collection_name + ".dat"), _logger);

            if(!tree.is_valid())
            {
                nlohmann::json res;
                res["message"] = "Collection could not be opened";
                return res;
            }

            auto val = tree.find_range(lower, upper);

            nlohmann::json res, k;

            for(auto& pair : val)
            {
                nlohmann::json tmp;
                tmp["key"] = pair.first;
                tmp["value"] = pair.second;

                k.push_back(tmp);
            }

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
CW_GUID disk_controller<tkey, tvalue, compare, t>::read_value(std::string pool_name, std::string scheme_name,
                                                              std::string collection_name, tkey key, bool need_persist,
                                                              disk_controller::time_point_t time)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name, collection_name, key, need_persist, time](){

        if (need_persist)
        {
            lock_helper lock(_opened_dirs_files, true, "", _opened_mutex, _opened_cond_var);

            revert_helper rev(*this, time);

            if(!std::filesystem::exists(_root / pool_name))
            {
                nlohmann::json res;
                res["message"] = "Pool does not exist";
                return res;
            }

            if(!std::filesystem::exists(_root / pool_name / scheme_name))
            {
                nlohmann::json res;
                res["message"] = "Scheme does not exist";
                return res;
            }

            if(!std::filesystem::exists(_root / pool_name / scheme_name / (collection_name + ".dat")))
            {
                nlohmann::json res;
                res["message"] = "Collection does not exist";
                return res;
            }

            B_tree_disk<tkey, tvalue, compare, t> tree(_root / pool_name / scheme_name / (collection_name + ".dat"), _logger);

            if(!tree.is_valid())
            {
                nlohmann::json res;
                res["message"] = "Collection could not be opened";
                return res;
            }

            auto val = tree.at(key);

            if(!val)
            {
                nlohmann::json res;
                res["message"] = "No data with such key";
                return res;
            }

            nlohmann::json res;

            res["data"] = val.value().to_json();

            return res;
        } else
        {
            lock_helper lock(_opened_dirs_files, false, "", _opened_mutex, _opened_cond_var);

            if(!std::filesystem::exists(_root / pool_name))
            {
                nlohmann::json res;
                res["message"] = "Pool does not exist";
                return res;
            }

            lock_helper plock(_opened_dirs_files, false, pool_name, _opened_mutex, _opened_cond_var);

            if(!std::filesystem::exists(_root / pool_name / scheme_name))
            {
                nlohmann::json res;
                res["message"] = "Scheme does not exist";
                return res;
            }

            lock_helper slock(_opened_dirs_files, false, pool_name + '/' + scheme_name, _opened_mutex, _opened_cond_var);

            if(!std::filesystem::exists(_root / pool_name / scheme_name / (collection_name + ".dat")))
            {
                nlohmann::json res;
                res["message"] = "Collection does not exist";
                return res;
            }

            lock_helper clock(_opened_dirs_files, false, pool_name + '/' + scheme_name + '/' + collection_name, _opened_mutex, _opened_cond_var);

            B_tree_disk<tkey, tvalue, compare, t> tree(_root / pool_name / scheme_name / (collection_name + ".dat"), _logger);

            if(!tree.is_valid())
            {
                nlohmann::json res;
                res["message"] = "Collection could not be opened";
                return res;
            }

            auto val = tree.at(key);

            if(!val)
            {
                nlohmann::json res;
                res["message"] = "No data with such key";
                return res;
            }

            nlohmann::json res;

            res["data"] = val.value().to_json();

            return res;
        }



    }).then([this, id](auto fut){
        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, fut.get()));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::remove(std::string pool_name, std::string scheme_name,
                                                          std::string collection_name, tkey key)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name, collection_name, key](){
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

        auto val = tree.at(key);

        if(!val)
            return std::string("Data does not exist");

        res = val;

        if (res)
        {
            tree.erase(key);

            std::lock_guard hlock(_history_mut);
            _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(
                    new in_disk_data_remove_operation(_index_gen(), pool_name, scheme_name, collection_name,
                                                      std::make_pair(key, val.value())))));
        } else
        {
            return std::string("Data does not exist");
        }
        return std::string("Data was successfully updated");

    }).then([this, id](auto fut){
        nlohmann::json res;
        res["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, res));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::update(std::string pool_name, std::string scheme_name,
                                                          std::string collection_name, tkey key, tvalue value)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name, collection_name, key, value](){
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

        res = tree.update(std::make_pair(key, value));


        if (res)
        {
            std::lock_guard hlock(_history_mut);
            _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(
                    new in_disk_data_update_operation(_index_gen(), pool_name, scheme_name, collection_name,
                                                      std::make_pair(key, value)))));
        } else
        {
            return std::string("Data does not exist");
        }
        return std::string("Data was successfully updated");

    }).then([this, id](auto fut){
        nlohmann::json res;
        res["message"] = fut.get();

        std::lock_guard lock(_map_mut);

        _request_result.insert(std::make_pair(id, res));
    });

    return id;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
CW_GUID disk_controller<tkey, tvalue, compare, t>::insert(std::string pool_name, std::string scheme_name,
                                                          std::string collection_name, tkey key, tvalue value)
{
    CW_GUID id;

    boost::async(boost::launch::async, [this, pool_name, scheme_name, collection_name, key, value](){
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

        res = tree.insert(std::make_pair(key, value));


        if (res)
        {
            std::lock_guard hlock(_history_mut);
            _history.push(std::make_pair(std::chrono::utc_clock::now(), std::shared_ptr<in_disk_operation>(
                    new in_disk_data_insert_operation(_index_gen(), pool_name, scheme_name, collection_name,
                                                      std::make_pair(key, value)))));
        } else
        {
            return std::string("Data already exists");
        }
        return std::string("Data was successfully inserted");

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

            size_t length = 0;

            unsigned int ind = _index_gen();

            remove_dir(_root / pool_name / scheme_name / (collection_name + ".dat"), _bucket, ind);
            remove_dir(_root / pool_name / scheme_name / (collection_name + "_data.bin"), _bucket, ind);

            std::lock_guard hlock(_history_mut);

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


        if (std::filesystem::exists(_root / pool_name / scheme_name / (collection_name + ".dat")))
        {
            return std::string("Collection already exists");
        }

        B_tree_disk<tkey, tvalue, compare, t> tree(_root / pool_name / scheme_name / (collection_name + ".dat"), _comparer, _logger);

        if (tree.is_valid())
        {
            std::lock_guard hlock(_history_mut);

            size_t length = 0;

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

            auto ind = _index_gen();

            remove_dir(_root / pool_name / scheme_name, _bucket, ind);

            std::lock_guard hlock(_history_mut);

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

            auto ind = _index_gen();

            remove_dir(_root / pool_name, _bucket, ind);

            std::lock_guard hlock(_history_mut);

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
disk_controller<tkey, tvalue, compare, t>::disk_controller(const std::string& path, compare comparer, logger* logger) : _logger(logger), _comparer(comparer)
{
    _root = std::filesystem::weakly_canonical("./" + path);
    _bucket = std::filesystem::weakly_canonical("./" + path + "_bucket");

    std::filesystem::remove_all(_bucket);
}


#endif //MP_OS_DISK_CONTROLLER_H
