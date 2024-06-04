//
// Created by Des Caldnd on 5/29/2024.
//

#ifndef MP_OS_B_TREE_DISK_H
#define MP_OS_B_TREE_DISK_H

#include <iterator>
#include <utility>
#include <array>
#include <concepts>
#include <stack>
#include <fstream>
#include <initializer_list>
#include <logger_guardant.h>
#include <optional>
#include <cstddef>

#include <controller_int.h>
#include <operation.h>



template <serializable tkey, serializable tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 2>
class B_tree_disk final : public logger_guardant, private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<const tkey, tvalue>;

private:

    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;

    // region comparators declaration

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;

    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;

    // endregion comparators declaration

public:

    struct btree_disk_node
    {
        size_t size; // кол-во заполненных ячеек
        bool _is_leaf;
        size_t position_in_disk;
        std::array<tree_data_type, maximum_keys_in_node + 1> keys;
        std::array<size_t, maximum_keys_in_node + 2> pointers;//номер байта в файле
        void serialize(std::fstream& stream, std::fstream& stream_for_data) const;

        static btree_disk_node deserialize(std::fstream& stream, std::fstream& stream_for_data);

        btree_disk_node(bool is_leaf);
        btree_disk_node();
    };

private:

    friend btree_disk_node;

    logger* _logger;

    std::fstream _file_for_tree;

    std::fstream _file_for_key_value;

    btree_disk_node _root;

    size_t _position_root;//

    btree_disk_node _current_node;

    logger* get_logger() const noexcept override;



public:

    static size_t _count_of_node;//только растет

    // region constructors declaration

    explicit B_tree_disk(const std::string& file_path, const compare& cmp = compare(), logger* logger = nullptr);


    // endregion constructors declaration

    // region five declaration

    B_tree_disk(B_tree_disk&& other) noexcept =default;
    B_tree_disk& operator=(B_tree_disk&& other) noexcept =default;

    B_tree_disk(const B_tree_disk& other) =delete;
    B_tree_disk& operator=(const B_tree_disk& other) =delete;

    ~B_tree_disk() noexcept = default;

    // endregion five declaration

    // region iterators declaration

    class btree_disk_const_iterator
    {
        std::stack<std::pair<size_t , size_t>> _path;
        size_t _index;
    public:
        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ptrdiff_t;

        using self = btree_disk_const_iterator;

        friend class B_tree_disk;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        explicit btree_disk_const_iterator(const std::stack<std::pair<size_t, size_t>>& path = std::stack<std::pair<size_t, size_t>>(), size_t index = 0);
    };

    friend class btree_disk_const_iterator;

    std::optional<tvalue> at(const tkey&) const;

    btree_disk_const_iterator begin() const;
    btree_disk_const_iterator end() const;

    std::vector<tree_data_type_const> find_range(const tkey& lower, const tkey& upper) const;

    /*
     * Does nothing if key exists
     * Second return value is true, when inserted
     */
    bool insert(const tree_data_type& data);

    /*
     * Updates value if key exists
     */
    bool update(const tree_data_type& data);

    /*
     * Return true if deleted
     */
    bool erase(const tkey& key);

    std::stack<std::pair<size_t, size_t>> find_path(const tkey& key);

public:

    btree_disk_node disk_read(size_t position);



    void disk_write(btree_disk_node& node);
private:

    std::pair<size_t, bool> find_index(const tkey &key, btree_disk_node& node) const noexcept;

    void insert_array(btree_disk_node& node, size_t right_node, const tree_data_type& data, size_t index) noexcept;

    void split_node(std::stack<std::pair<size_t, size_t>>& path, btree_disk_node& node, size_t& index);

    btree_disk_node remove_array(btree_disk_node& node, size_t index, bool remove_left_ptr = true) noexcept;

    void rebalance_node(std::stack<std::pair<size_t, size_t>> &path, btree_disk_node& node,size_t &index);



};

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    auto path = find_path(key);

    if((path.top().first == 0 && path.top().second == 0) || path.top().second == 2*t+3)
    {
        return false;
    }

    btree_disk_node current_node = disk_read(path.top().first);
    size_t index = path.top().second;
    if(!current_node._is_leaf)
    {

        path.push(std::make_pair(current_node.pointers[index], index));

        btree_disk_node node = disk_read(path.top().first);

        while(!node._is_leaf)
        {
            path.push(std::make_pair(node.pointers[node.size], node.size));
        }

        node = disk_read(path.top().first);

        std::swap(current_node.keys[index], node.keys[node.size - 1]);
        index = node.size - 1;
    }

    current_node = disk_read(path.top().first);

    remove_array(current_node, index);

    btree_disk_node n = disk_read(path.top().first);

    while(!path.empty())
    {
        btree_disk_node tmp = disk_read(path.top().first);
        if(tmp.size >= minimum_keys_in_node)
        {
            break;
        }

        rebalance_node(path, n, index);
    }

    if(n.size = 0)//////TODO мб зануление везде продумать на position 0
    {

    }

    auto res = find_path(n.keys[index].first);

    if(res.top().first != 0)
    {
        return true;
    }

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::rebalance_node(std::stack<std::pair<size_t, size_t>> &path, btree_disk_node& node,size_t &index)
{
    if (path.empty())
        return;
    if (path.size() == 1)
    {
        btree_disk_node nodes = disk_read(path.top().first);
        if (node.size == 0)
        {
            path.top().first = 0;
            path.top().second = 0;
            node = nullptr;
        }
        path.pop();
        return;
    } else
    {
        auto pair = path.top();
        auto current_node = disk_read(pair.first);
        path.pop();
        auto nodes = disk_read(path.top().first);

        bool need_key = true;

        if (pair.second > 0)
        {
            btree_disk_node brother = disk_read(nodes.pointers[pair.second - 1]);

            if (brother.size > minimum_keys_in_node)
            {
                auto tmp = nodes.keys[pair.second - 1];
                std::swap(nodes.keys[pair.second - 1], brother.keys[brother.size - 1]);

                auto tmp_ptr = remove_array(brother, brother.size - 1, false);

                std::swap(current_node.pointers[0], tmp_ptr.position_in_disk);

                insert_array(current_node, tmp_ptr.position_in_disk, std::move(tmp), 0);

                ++index;

                need_key = false;
            }
        }

        if (need_key && pair.second < nodes.size)
        {
            btree_disk_node brother = disk_read(nodes.pointers[pair.second + 1]);

            if (brother.size > minimum_keys_in_node)
            {
                auto tmp = nodes.keys[pair.second];
                std::swap(nodes.keys[pair.second], brother.keys[0]);

                auto tmp_ptr = remove_array(brother, 0);

                insert_array(current_node, tmp_ptr.position_in_disk, std::move(tmp), current_node->size);

                need_key = false;
            }
        }

        if (need_key)
        {
            btree_disk_node parent = disk_read(path.top().first);
            btree_disk_node left = pair.second > 0 ? disk_read(parent.pointers[pair.second - 1]) : current_node;
            btree_disk_node right = pair.second > 0 ? current_node : disk_read(parent.pointers[pair.second + 1]);
            size_t parent_index = pair.second > 0 ? pair.second - 1 : pair.second;
            if (pair.second > 0)
            {
                node = left;
                index += left.size + 1;
            }

            auto tmp_data = parent.keys[parent_index];

            insert_array(left, right.pointers[0], std::move(tmp_data), left->size);

            for (size_t i = 0; i < right.size; ++i)
            {
                insert_array(left, right.pointers[i + 1], std::move(right.keys[i]), left->size);
            }

            remove_array(parent, parent_index, false);

            btree_disk_node top = disk_read(path.top().first);

            if (path.size() == 1 && top.size == 0)
            {
                path.pop();
                _root = left;
            }
        }
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::remove_array(btree_disk_node& node, size_t index, bool remove_left_ptr) noexcept
{
    btree_disk_node res = remove_left_ptr ? disk_read(node.pointers[index]) : disk_read(node.pointers[index + 1]);

    bool is_left_terminate = node._is_leaf;

    if (is_left_terminate && index == 0 && remove_left_ptr)
    {
        res = btree_disk_node(true);
    }

    for(size_t i = index; i < node.size - 1; ++i)
    {
        std::swap(node.keys[i], node.keys[i + 1]);
        if (remove_left_ptr)
            node.pointers[i] = node.pointers[i + 1];
        else
            node.pointers[i + 1] = node.pointers[i + 2];
    }

    if (remove_left_ptr)
    {
        node.pointers[node.size - 1] = node.pointers[node.size];
    }

    --node.size;

    if (is_left_terminate)
        node.pointers[0] = nullptr;

    return res;
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::insert(const B_tree_disk::tree_data_type &data)
{
    auto path = find_path(data.first);
    if(path.top().first == 0 && path.top().second == 0)//дерево пустое
    {
        auto node = disk_read(1);
        node.size++;
        node.keys[0] = std::move(data);
        disk_write(node);
        return true;
    }

    btree_disk_node current_node = disk_read(path.top().first);

    auto [ind, found] = find_index(data.first, current_node);

    if(found)
    {
        return true;
    }

    insert_array(current_node, 0, data, ind);

    disk_write(current_node);

    path = find_path(data.first);

    btree_disk_node node = disk_read(path.top().first);
    while(!path.empty())
    {

        if(node.size <= maximum_keys_in_node)
        {
            break;
        }

        split_node(path, node, ind);
    }

    auto res = find_path(node.keys[ind].first);

    if(res.top().first != 0)
    {
        return true;
    }

    return false;

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::split_node(std::stack<std::pair<size_t, size_t>>& path, btree_disk_node& node, size_t& index)
{
    btree_disk_node new_node(true);

    if (path.size() == 1)
    {

        btree_disk_node tmp(false);

        disk_write(tmp);

        _root = disk_read(_position_root);

        _root = std::exchange(tmp, _root);

        _root.pointers[0] = _position_root;

        _position_root = _root.position_in_disk;

        path.pop();

        path.push(std::make_pair(_root.position_in_disk, 0));
        path.push(std::make_pair(tmp.position_in_disk, size_t(0)));

    }

    btree_disk_node current_node = disk_read(path.top().first);

    size_t separator = current_node.size / 2;

    auto pair = path.top();

    btree_disk_node node_ptr = disk_read(pair.first);

    path.pop();

    btree_disk_node temp = disk_read(path.top().first);

    insert_array(temp, new_node.position_in_disk, std::move(node_ptr.keys[separator - 1]), pair.second);

    disk_write(temp);

    new_node.pointers[0] = node._is_leaf ? 0 : node_ptr.pointers[separator];
    new_node.size = node_ptr.size - separator;

    for (size_t i = separator; i < node_ptr.size; ++i)
    {
        new_node.keys[i - separator] = std::move(node_ptr.keys[i]);
        new_node.pointers[i - separator + 1] = node_ptr.pointers[i + 1];
    }

    if (node_ptr._is_leaf)
    {
        new_node.pointers[0] = 0;
    }
    node_ptr.size = separator - 1;

    if (node.position_in_disk == node_ptr.position_in_disk)//мб вообще убрать
    {
        if (index == separator - 1)
        {
            node = path.top().first;
            index = pair.second;
        } else if (index >= separator)
        {
            node = new_node;
            index -= separator;
        }
    }

    disk_write(node_ptr);
    disk_write(new_node);
    disk_write(node);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::insert_array(btree_disk_node& node, size_t right_node, const tree_data_type& data, size_t index) noexcept
{
    for (size_t i = node.size; i > index; --i)
    {
        node.keys[i] = std::move(node.keys[i-1]);
        node.pointers[i + 1] = node.pointers[i];
    }

    node.keys[index] = std::move(data);
    node.pointers[index + 1] = right_node;
    ++node.size;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::stack<std::pair<size_t, size_t>>  B_tree_disk<tkey, tvalue, compare, t>::find_path(const tkey& key)
{
    btree_disk_node node = disk_read(_position_root);
    if(node.size == 0)
    {
        std::stack<std::pair<size_t, size_t>> path;
        path.push(std::make_pair(size_t(0), size_t(0)));//по нулям - значит пустое дерево
        return path;
    }
    else
    {
        std::stack<std::pair<size_t, size_t>> path; // node index

        auto [ind, found] = find_index(key, node);

        while(!found && !node._is_leaf)
        {
            path.push(std::make_pair(node.position_in_disk, ind));

            node = disk_read(node.pointers[ind]);

            auto tmp = find_index(key, node);
            ind = tmp.first;
            found = tmp.second;
        }

        if(found)
        {
            path.push(std::make_pair(node.position_in_disk, ind));
            return path;
        }
        else
        {
            path.push(std::make_pair(node.position_in_disk, 2*t+1));///значит мы знаем ноду куда вставлять
            return path;
        }
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<size_t, bool> B_tree_disk<tkey, tvalue, compare, t>::find_index(const tkey &key, btree_disk_node& node) const noexcept
{
    auto beg = node.keys.begin(), end = beg + node.size;

    while (end > beg)
    {
        auto mid = beg + std::distance(beg, end) / 2;

        if (compare_keys(key, mid->first))
        {
            end = mid;
        } else if (compare_keys(mid->first, key))
        {
            beg = mid + 1;
        } else
        {
            return std::make_pair(std::distance(node.keys.begin(), mid), true);
        }
    }

    if (beg == node.keys.begin() + node.size)
        return std::make_pair(node.size, false);

    if (compare_keys(key, beg->first))
        return std::make_pair(std::distance(node.keys.begin(), beg), false);
    else
        return std::make_pair(std::distance(node.keys.begin(), beg) + 1, false);
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::serialize(std::fstream &stream, std::fstream& stream_for_data) const
{
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

    stream.write(reinterpret_cast<const char*>(&_is_leaf), sizeof(bool));

    stream.write(reinterpret_cast<const char*>(&position_in_disk), sizeof(size_t));

    for (size_t i = 0, end = keys.size(); i < end; ++i)
    {
        size_t key_type_size = keys[i].first.serialize();

        std::streampos pos_key = stream_for_data.tellp();

        stream.write(reinterpret_cast<const char*>(&pos_key), sizeof(size_t));

        stream.write(reinterpret_cast<const char*>(&key_type_size), sizeof(size_t));

        stream_for_data.write(reinterpret_cast<const char*>(&(keys[i].first)), key_type_size);

//        if(key_type_size > static_cast<size_t>(std::numeric_limits<long long>::max()))
//        {
//            throw std::runtime_error("key size is too large to be represented as long long");
//        }

        size_t value_type_size = keys[i].second.serialize();

        std::streampos pos_value = stream_for_data.tellp();

        stream.write(reinterpret_cast<const char*>(&pos_value), sizeof(size_t));

        stream.write(reinterpret_cast<const char*>(&value_type_size), sizeof(size_t));

        stream_for_data.write(reinterpret_cast<const char*>(&(keys[i].second)), value_type_size);

    }

    size_t pointers_size = pointers.size();

    stream.write(reinterpret_cast<const char*>(&pointers_size), sizeof(pointers_size));

    for (size_t i = 0, end = pointers.size(); i < end; ++i)
    {
        stream.write(reinterpret_cast<const char*>(&pointers[i]), sizeof(pointers[i]));
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::disk_write(btree_disk_node& node)
{
    size_t amount_bytes = 3*sizeof(size_t) + sizeof(bool) + (4*sizeof(size_t) ) * (maximum_keys_in_node + 1) + sizeof(size_t)*(maximum_keys_in_node + 2);

    size_t position_size_t = amount_bytes * node.position_in_disk;

//    if (position_size_t > static_cast<size_t>(std::numeric_limits<std::streampos>::max()))
//    {
//        throw std::runtime_error("Position size_t is too large to be represented as std::streampos.");
//    }

    auto position = static_cast<std::streampos>(position_size_t);
    _file_for_tree.seekp(position);

    node.serialize(_file_for_tree, _file_for_key_value);

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::deserialize(std::fstream &stream, std::fstream& stream_for_data)
{
    btree_disk_node node(true);


    stream.read(reinterpret_cast<char*>(&node.size), sizeof(size_t));


    stream.read(reinterpret_cast<char*>(&node._is_leaf), sizeof(bool));


    stream.read(reinterpret_cast<char*>(&node.position_in_disk), sizeof(size_t));


    size_t mostly_move = node.keys.size();

    for (size_t i = 0; i < node.size; ++i)
    {
        size_t key_type_size;
        size_t pos_key;

        stream.read(reinterpret_cast<char*>(&pos_key), sizeof(size_t));
        stream.read(reinterpret_cast<char*>(&key_type_size), sizeof(size_t));

        tkey key;
        stream_for_data.seekg(pos_key);
        stream_for_data.read(reinterpret_cast<char*>(&key), key_type_size);

        size_t value_type_size;
        size_t pos_value;

        stream.read(reinterpret_cast<char*>(&pos_value), sizeof(size_t));
        stream.read(reinterpret_cast<char*>(&value_type_size), sizeof(size_t));

        tvalue value;
        stream_for_data.seekg(pos_value);
        stream_for_data.read(reinterpret_cast<char*>(&value), value_type_size);

        node.keys[i] = {key, value};

        mostly_move--;
    }

    size_t moving_in_file = mostly_move * 4*sizeof(size_t);

    stream.seekg(moving_in_file, std::ios::cur);

    size_t pointers_size;
    stream.read(reinterpret_cast<char*>(&pointers_size), sizeof(size_t));

    for (size_t i = 0; i < pointers_size; ++i)
    {
        stream.read(reinterpret_cast<char*>(&node.pointers[i]), sizeof(size_t));
    }

    return std::move(node);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::disk_read(size_t node_position)
{
    //btree_disk_node node;

    size_t amount_bytes = 3 * sizeof(size_t) + sizeof(bool) + (4 * sizeof(size_t)) * (maximum_keys_in_node + 1) + sizeof(size_t) * (maximum_keys_in_node + 2);

    size_t position_size_t = amount_bytes * node_position;

//    if (position_size_t > static_cast<size_t>(std::numeric_limits<std::streampos>::max()))
//    {
//        throw std::runtime_error("Position size_t is too large to be represented as std::streampos.");
//    }

//    auto position = static_cast<std::streampos>(position_size_t);
    //auto position = static_cast<std::streampos>(position_size_t);
    _file_for_tree.seekg(position_size_t);

    btree_disk_node node = node.deserialize(_file_for_tree, _file_for_key_value);

    return node;
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node(bool is_leaf) : _is_leaf(is_leaf), size(0), position_in_disk(_count_of_node)
{
    _count_of_node++;
    for(size_t i = 0, end = pointers.size(); i < end; ++i)
    {
        pointers[i] = 0;
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node() : _is_leaf(true), size(0), position_in_disk(_count_of_node)
{
    _count_of_node++;
    for(size_t i = 0, end = pointers.size(); i < end; ++i)
    {
        pointers[i] = 0;
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
logger *B_tree_disk<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return _logger;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::compare_pairs(const B_tree_disk::tree_data_type &lhs,
                                                            const B_tree_disk::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree_disk<tkey, tvalue, compare, t>::_count_of_node = 0;

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::B_tree_disk(const std::string& file_path, const compare& cmp, logger* logger): _logger(logger), compare(cmp)
{
//    _file_for_tree.open(file_path, std::ios::binary | std::ios::in | std::ios::out);
//
//    if(!_file_for_tree.is_open())
//    {
//        throw std::runtime_error("Couldn't open file " + file_path);
//    }
//
//    std::string filename_for_file_key_value = file_path + "_data";
//
//    _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::in | std::ios::out);
//
//    if(!_file_for_key_value.is_open())
//    {
//        throw std::runtime_error("Couldn't open file " + filename_for_file_key_value);
//    }


    _file_for_tree.open(file_path, std::ios::binary | std::ios::out);

    if (!_file_for_tree.is_open()) {


    }

    if (!_file_for_tree.is_open()) {
        throw std::runtime_error("Couldn't open file " + file_path);
    }

    size_t last_index = file_path.find_last_of('.');
    std::string raw_name = file_path.substr(0, last_index);
    std::string filename_for_file_key_value = raw_name + "_data.bin";

    _file_for_key_value.clear();
    _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::out);
    _file_for_key_value.close();
    _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::in | std::ios::out);


    if (!_file_for_key_value.is_open()) {
        _file_for_key_value.clear();
        _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::out);
        _file_for_key_value.close();
        _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::in | std::ios::out);
    }

    if (!_file_for_key_value.is_open()) {
        throw std::runtime_error("Couldn't open file " + filename_for_file_key_value);
    }

    _count_of_node = 1;

    _root = btree_disk_node(true);

    _position_root = 1;

    disk_write(_root);


}




#endif //MP_OS_B_TREE_DISK_H
