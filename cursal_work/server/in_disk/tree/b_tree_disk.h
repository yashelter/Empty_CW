
#ifndef B_DISK_TREE_B_TREE_DISK_H
#define B_DISK_TREE_B_TREE_DISK_H

#include <iterator>
#include <utility>
#include <vector>
#include <concepts>
#include <stack>
#include <fstream>
#include <initializer_list>
#include <optional>
#include <cstddef>
#include <filesystem>

#include "controller_int.h"



template <serializable tkey, serializable tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 2>
class B_tree_disk final : private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<tkey, tvalue>;

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
        std::vector<tree_data_type> keys;
        std::vector<size_t> pointers;
        void serialize(std::fstream& stream, std::fstream& stream_for_data) const;

        static btree_disk_node deserialize(std::fstream& stream, std::fstream& stream_for_data);

        btree_disk_node(bool is_leaf);
        btree_disk_node();
    };

private:

    friend btree_disk_node;

    //logger* _logger;

    std::fstream _file_for_tree;

    std::fstream _file_for_key_value;

//    btree_disk_node _root;

public:

    size_t _position_root;//

private:


    btree_disk_node _current_node;

    //logger* get_logger() const noexcept override;



public:

    static size_t _count_of_node;//только растет

    // region constructors declaration

    explicit B_tree_disk(const std::string& file_path, const compare& cmp = compare(), void* logger = nullptr);


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
        B_tree_disk<tkey,tvalue, compare, t>& _tree;
    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ptrdiff_t;

        using self = btree_disk_const_iterator;

        friend class B_tree_disk;

        value_type operator*() noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(self& other) noexcept;
        bool operator!=(self& other) noexcept;

        explicit btree_disk_const_iterator(B_tree_disk<tkey, tvalue, compare, t>& tree, const std::stack<std::pair<size_t, size_t>>& path = std::stack<std::pair<size_t, size_t>>(), size_t index = 0);

    };

    friend class btree_disk_const_iterator;

    std::optional<tvalue> at(const tkey&);//либо пустое, либо tvalue//std::nullopt

    btree_disk_const_iterator begin();
    btree_disk_const_iterator end() ;

    //std::vector<tree_data_type_const> find_range(const tkey& lower, const tkey& upper) const;

    std::pair<btree_disk_const_iterator, btree_disk_const_iterator> find_range(const tkey& lower, const tkey& upper, bool include_lower = true, bool include_upper = false);

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

    bool is_valid() const noexcept;


    std::pair<std::stack<std::pair<size_t, size_t>>, std::pair<size_t, bool>>  find_path(const tkey& key);

public:

    btree_disk_node disk_read(size_t position);


    void check_tree(size_t pos, size_t depth);

    void disk_write(btree_disk_node& node);
private:

    std::pair<size_t, bool> find_index(const tkey &key, btree_disk_node& node) const noexcept;

    void insert_array(btree_disk_node& node, size_t right_node, const tree_data_type& data, size_t index) noexcept;

    void split_node(std::stack<std::pair<size_t, size_t>>& path);

    btree_disk_node remove_array(btree_disk_node& node, size_t index, bool remove_left_ptr = true) noexcept;

    void rebalance_node(std::stack<std::pair<size_t, size_t>> &path, btree_disk_node& node,size_t &index);

    void print_root_position() noexcept;

};

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::is_valid() const noexcept
{
    return _file_for_tree.is_open() && _file_for_key_value.is_open();
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    auto path = find_path(key);

    if(path.second.second == false)
    {
        return false;
    }

    auto path_path = path.first;
    size_t index = path.second.first;

    btree_disk_node current_node = disk_read(path_path.top().first);

    if(!current_node._is_leaf)
    {
        btree_disk_node tmp = disk_read(path_path.top().first);

        path_path.push(std::make_pair(tmp.pointers[index], index));

//        btree_disk_node node = disk_read(tmp.pointers[0]);
//        path_path.push(std::make_pair(tmp.pointers[0], 0));

        btree_disk_node node = disk_read(path_path.top().first);

        while(!node._is_leaf)
        {
            path_path.push(std::make_pair(node.pointers[node.size], node.size));
            node = disk_read(path_path.top().first);
        }

        node = disk_read(path_path.top().first);

        std::swap(tmp.keys[index], node.keys[node.size - 1]);

        disk_write(tmp);

        disk_write(node);

//        tmp = disk_read(path_path.top().first);

        index = node.size - 1;

    }

    current_node = disk_read(path_path.top().first);

    remove_array(current_node, index);

    disk_write(current_node);

    btree_disk_node n = disk_read(path_path.top().first);

    while(!path_path.empty())
    {
        btree_disk_node tmp = disk_read(path_path.top().first);
        if(tmp.size >= minimum_keys_in_node)
        {
            break;
        }

        rebalance_node(path_path, n, index);
    }

    if(n.size == 0)
    {
        return true;
    }

    auto res = find_path(key);

    if(res.second.second == false)
    {
        return true;
    }

    return false;

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::print_root_position() noexcept
{
    if(!_file_for_tree.is_open())
    {
        return;
    }
    _file_for_tree.seekg(sizeof(size_t), std::ios::beg);
    _file_for_tree.write(reinterpret_cast<const char*>(&_position_root), sizeof(size_t));
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::rebalance_node(std::stack<std::pair<size_t, size_t>> &path, btree_disk_node& node,size_t &index)
{
    if (path.empty())
    {
        return;
    }
    if (path.size() == 1)
    {
        btree_disk_node top = disk_read(path.top().first);
        if (top.size == 0)
        {
//            path.top().first = 0;
//            path.top().second = 0;
            node.size = 0;
//            _root = node;
//            _position_root = 0;
            //disk_write(node);
        }
        path.pop();
        return;
    } else
    {
        auto pair = path.top();
        auto current_node = disk_read(pair.first);
        path.pop();


        bool need_key = true;

        if (pair.second > 0)
        {
            auto nodes = disk_read(path.top().first);
            btree_disk_node brother = disk_read(nodes.pointers[pair.second - 1]);

            if (brother.size > minimum_keys_in_node)
            {
                auto tmp = nodes.keys[pair.second - 1];
                std::swap(nodes.keys[pair.second - 1], brother.keys[brother.size - 1]);

                disk_write(nodes);

                auto tmp_ptr = remove_array(brother, brother.size - 1, false);

                disk_write(brother);
                std::swap(current_node.pointers[0], tmp_ptr.position_in_disk);

//                disk_write(current_node);
                //disk_write(tmp_ptr);

                insert_array(current_node, tmp_ptr.position_in_disk, std::move(tmp), 0);

                ++index;

                disk_write(current_node);
                //disk_write(tmp_ptr);

                need_key = false;
            }
        }

        btree_disk_node nodes = disk_read(path.top().first);

        if (need_key && pair.second < nodes.size)
        {
            btree_disk_node brother = disk_read(nodes.pointers[pair.second + 1]);

            if (brother.size > minimum_keys_in_node)
            {
                auto tmp = nodes.keys[pair.second];
                std::swap(nodes.keys[pair.second], brother.keys[0]);

                disk_write(nodes);

                auto tmp_ptr = remove_array(brother, 0);
//                remove_array(brother, 0);

                disk_write(brother);
                //disk_write(tmp_ptr);

                insert_array(current_node, tmp_ptr.position_in_disk, std::move(tmp), current_node.size);

                //disk_write(tmp_ptr);
                disk_write(current_node);

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

            insert_array(left, right.pointers[0], std::move(tmp_data), left.size);

            for (size_t i = 0; i < right.size; ++i)
            {
                insert_array(left, right.pointers[i + 1], std::move(right.keys[i]), left.size);
            }

            remove_array(parent, parent_index, false);



//            btree_disk_node top = disk_read(path.top().first);

            if (path.size() == 1 && parent.size == 0)
            {
                path.pop();
//                _root = left;
                _position_root = left.position_in_disk;
                print_root_position();
            }

            disk_write(left);
//            disk_write(right);
            disk_write(parent);
//            disk_write(node);

        }
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::remove_array(btree_disk_node& node, size_t index, bool remove_left_ptr) noexcept
{
    btree_disk_node res = remove_left_ptr ? disk_read(node.pointers[index]) : disk_read(node.pointers[index + 1]);///////////////////////////////////////

    bool is_left_terminate = node._is_leaf;

    if (is_left_terminate && index == 0 && remove_left_ptr)
    {
        res = btree_disk_node(true);///////////////////////////////////////////////
        res.position_in_disk = 0;
    }

//    for(size_t i = index; i < node.size - 1; ++i)
//    {
//        std::swap(node.keys[i], node.keys[i + 1]);
//        if (remove_left_ptr) {
//            node.pointers[i] = node.pointers[i + 1];
//        }
//        else {
//            node.pointers[i + 1] = node.pointers[i + 2];
//        }
//
//
//    }
//    disk_write(node);

//    if (remove_left_ptr)
//    {
//        node.pointers[node.size - 1] = node.pointers[node.size];
//    }

    node.keys.erase(node.keys.begin() + index);

    node.pointers.erase(node.pointers.begin() + index + (remove_left_ptr ? 0 : 1));

    --node.size;

    disk_write(node);

//    if (is_left_terminate)
//        node.pointers[0] = 0;

    if(is_left_terminate)
    {
        if(node.pointers.empty())
        {
            node.pointers.push_back(0);
        }
        else
        {
            node.pointers[0] = 0;
        }
    }

    disk_write(node);

    return res;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::update(const B_tree_disk::tree_data_type &data)
{
    auto path = find_path(data.first);
    auto path_path = path.first;

    if(path.second.second == false)
    {
        return false;
    }

    btree_disk_node current_node = disk_read(path_path.top().first);

    auto [ind, found] = find_index(data.first, current_node);

    if(!found)
    {
        return false;
    }

    current_node.keys[ind].second = data.second;

    disk_write(current_node);

    return true;

}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::insert(const B_tree_disk::tree_data_type &data)
{
    auto path = find_path(data.first);
    auto path_path = path.first;



    if(path.second.second == true)
    {
        return false;
    }

    auto root = disk_read(_position_root);

    if(root.size == 0)
    {
        root.size++;
//        root.keys[0] = std::move(data);
        root.keys.emplace_back(std::move(data));
        root.pointers.emplace_back(size_t(0));
//        std::cout << _file_for_tree.tellp() << std::endl;
        disk_write(root);
//        std::cout << _file_for_tree.tellp() << std::endl;
        return true;
    }

    btree_disk_node current_node = disk_read(path_path.top().first);

    auto [ind, found] = find_index(data.first, current_node);

    if(found)
    {
        return false;
    }

    insert_array(current_node, 0, data, ind);

    disk_write(current_node);//think

    btree_disk_node tmp = disk_read(path_path.top().first);

    while(!path_path.empty())
    {
        btree_disk_node temp = disk_read(path_path.top().first);
        if(temp.size <= maximum_keys_in_node)
        {
            break;
        }
        split_node(path_path);
    }

//    std::cout << _file_for_tree.tellp() << std::endl;

    return true;



//    path = find_path(data.first);
//
//    btree_disk_node node = disk_read(path.top().first);
//    while(!path.empty())
//    {
//
//        if(node.size <= maximum_keys_in_node)
//        {
//            break;
//        }
//
//        split_node(path, node, ind);
//    }
//
//    auto res = find_path(node.keys[ind].first);
//
//    if(res.top().first != 0)
//    {
//        return true;
//    }
//
//    return false;

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::split_node(std::stack<std::pair<size_t, size_t>>& path)
{

    if (path.size() == 1)
    {

        btree_disk_node tmp(false);

        //disk_write(tmp);

        btree_disk_node root = disk_read(_position_root);

        root = std::exchange(tmp, root);

        root.pointers[0] = _position_root;

        disk_write(root);

        disk_write(tmp);

        _position_root = root.position_in_disk;

        print_root_position();

        path.pop();

        path.push(std::make_pair(root.position_in_disk, 0));
        path.push(std::make_pair(tmp.position_in_disk, size_t(0)));

    }

    auto pair = path.top();

    btree_disk_node node_ptr = disk_read(pair.first);

    btree_disk_node new_node(node_ptr._is_leaf);

    size_t separator = node_ptr.size / 2;



    path.pop();

    btree_disk_node temp = disk_read(path.top().first);

    insert_array(temp, new_node.position_in_disk, std::move(node_ptr.keys[separator - 1]), pair.second);

    disk_write(temp);

    new_node.pointers[0] = node_ptr._is_leaf ? 0 : node_ptr.pointers[separator];
    new_node.size = node_ptr.size - separator;

    for (size_t i = separator; i < node_ptr.size; ++i)
    {
//        new_node.keys[i - separator] = std::move(node_ptr.keys[i]);
//        new_node.pointers[i - separator + 1] = node_ptr.pointers[i + 1];
        new_node.keys.emplace_back(std::move(node_ptr.keys[i]));
        new_node.pointers.push_back(node_ptr.pointers[i+1]);
    }

    while(node_ptr.keys.size() > separator - 1)
    {
        node_ptr.keys.pop_back();
        node_ptr.pointers.pop_back();
    }

    if (node_ptr._is_leaf)
    {
        new_node.pointers[0] = 0;
    }
    node_ptr.size = separator - 1;


    //btree_disk_node check = disk_read(1);
    disk_write(node_ptr);
    disk_write(new_node);
    //disk_write(node);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::insert_array(btree_disk_node& node, size_t right_node, const tree_data_type& data, size_t index) noexcept
{
//    for (size_t i = node.size; i > index; --i)
//    {
//        node.keys[i] = std::move(node.keys[i-1]);
//        node.pointers[i + 1] = node.pointers[i];
//    }

//    node.keys[index] = std::move(data);
//    node.pointers[index + 1] = right_node;
    node.keys.insert(node.keys.begin() + index, data);
    node.pointers.insert(node.pointers.begin() + index + 1, right_node);
    ++node.size;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<std::stack<std::pair<size_t, size_t>>, std::pair<size_t,bool>>  B_tree_disk<tkey, tvalue, compare, t>::find_path(const tkey& key)
{
    btree_disk_node node = disk_read(_position_root);
    if(node.size == 0)
    {
        std::stack<std::pair<size_t, size_t>> path;
        path.push(std::make_pair(_position_root, size_t(0)));//по нулям - значит пустое дерево
        return std::make_pair(std::move(path), std::make_pair(size_t(0), false));
    }
    else
    {
        std::stack<std::pair<size_t, size_t>> path; // node index

        path.push(std::make_pair(_position_root, size_t(0)));

        auto [ind, found] = find_index(key, node);

        while(!found && !node._is_leaf)
        {
            path.push(std::make_pair(node.pointers[ind], ind));

            node = disk_read(node.pointers[ind]);

            auto tmp = find_index(key, node);
            ind = tmp.first;
            found = tmp.second;
        }

        if(found)
        {
//            path.push(std::make_pair(node.position_in_disk, ind));
            return std::make_pair(path, std::make_pair(ind, true));
        }
        else
        {
            //path.push(std::make_pair(node.position_in_disk, 2*t+1));///значит мы знаем ноду куда вставлять
            btree_disk_node node2 = disk_read(path.top().first);
            return std::make_pair(path, std::make_pair(node2.size + 1, false));
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

    size_t pos = stream.tellg();

    stream.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

    stream.write(reinterpret_cast<const char*>(&_is_leaf), sizeof(bool));

    stream.write(reinterpret_cast<const char*>(&position_in_disk), sizeof(size_t));;

    stream_for_data.seekg(0, std::ios_base::end);

    for (size_t i = 0, end = size; i < end; ++i)
    {

        size_t pos_key = stream_for_data.tellg();

        stream.write(reinterpret_cast<const char*>(&pos_key), sizeof(size_t));

        keys[i].first.serialize(stream_for_data);


        size_t pos_value = stream_for_data.tellg();

        stream.write(reinterpret_cast<const char*>(&pos_value), sizeof(size_t));

        keys[i].second.serialize(stream_for_data);

    }

    for(size_t i = size; i < maximum_keys_in_node + 1; ++i)
    {
        size_t ttt = 0;
        stream.write(reinterpret_cast<const char*>(&ttt), sizeof(size_t));
        stream.write(reinterpret_cast<const char*>(&ttt), sizeof(size_t));
    }

    size_t pointers_size = pointers.size();

    stream.write(reinterpret_cast<const char*>(&pointers_size), sizeof(size_t));

    for (size_t i = 0, end = maximum_keys_in_node + 2; i < end; ++i)
    {
        if (i < pointers.size())
            stream.write(reinterpret_cast<const char*>(&(pointers[i])), sizeof(size_t));
        else
        {
            size_t zero = 0;
            stream.write(reinterpret_cast<const char*>(&(zero)), sizeof(size_t));
        }
    }

    stream.seekg(pos, std::ios_base::beg);
    size_t test;

    stream.read(reinterpret_cast<char*>(&test), sizeof(size_t));
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::disk_write(btree_disk_node& node)
{
    size_t amount_bytes = 3*sizeof(size_t) + sizeof(bool) + (2*sizeof(size_t) ) * (maximum_keys_in_node + 1) + sizeof(size_t)*(maximum_keys_in_node + 2);

    size_t position_size_t = amount_bytes * node.position_in_disk;

    _file_for_tree.seekg(position_size_t, std::ios_base::beg);

    node.serialize(_file_for_tree, _file_for_key_value);


}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::deserialize(std::fstream &stream, std::fstream& stream_for_data)
{
    //std::cout<<stream.tellg()<<std::endl;

//    std::cout << "Read: " <<stream.tellp() <<std::endl;

    btree_disk_node node(true);

    _count_of_node--;
    //node.size = 54;

    stream.read(reinterpret_cast<char*>(&(node.size)), sizeof(size_t));

    //std::cout<<stream.tellg()<<std::endl;


    stream.read(reinterpret_cast<char*>(&node._is_leaf), sizeof(bool));


    //std::cout<<stream.tellg()<<std::endl;

    stream.read(reinterpret_cast<char*>(&node.position_in_disk), sizeof(size_t));

    //std::cout<<stream.tellg()<<std::endl;

    size_t mostly_move = maximum_keys_in_node + 1;

    //std::cout << "=====================\n";

    for (size_t i = 0; i < node.size; ++i)
    {


        size_t pos_key;

        stream.read(reinterpret_cast<char*>(&pos_key), sizeof(size_t));

        //std::cout << pos_key << std::endl;

        stream_for_data.seekg(pos_key);

        tkey key = tkey::deserialize(stream_for_data);


        size_t pos_value;

        stream.read(reinterpret_cast<char*>(&pos_value), sizeof(size_t));

        //std::cout << pos_value << std::endl;

        stream_for_data.seekg(pos_value);

        tvalue value = tvalue::deserialize(stream_for_data);

        node.keys.emplace_back(key, value);

        mostly_move--;
    }

    //std::cout << "=====================\n";

    size_t moving_in_file = mostly_move * 2*sizeof(size_t);

    stream.seekg(moving_in_file, std::ios::cur);
//    stream.seekp(moving_in_file, std::ios::cur);

    //std::cout<<stream.tellg()<<std::endl;


    size_t pointers_size;
    stream.read(reinterpret_cast<char*>(&pointers_size), sizeof(size_t));

    //std::cout<<stream.tellg()<<std::endl;

    node.pointers.pop_back();
    for (size_t i = 0; i < pointers_size; ++i)
    {
        size_t tmp;
        stream.read(reinterpret_cast<char*>(&tmp), sizeof(size_t));
        //std::cout<<stream.tellg()<<std::endl;
        node.pointers.emplace_back(tmp);

    }

//    std::cout <<stream.tellp() << " " << node.position_in_disk <<std::endl;

    return node;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::disk_read(size_t node_position)
{
    size_t amount_bytes = 3 * sizeof(size_t) + sizeof(bool) + (2 * sizeof(size_t)) * (maximum_keys_in_node + 1) + sizeof(size_t) * (maximum_keys_in_node + 2);

    size_t position_size_t = amount_bytes * node_position;

    _file_for_tree.seekg(position_size_t, std::ios_base::beg);

    btree_disk_node node = node.deserialize(_file_for_tree, _file_for_key_value);

    return node;
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node(bool is_leaf) : _is_leaf(is_leaf), size(0), position_in_disk(_count_of_node)
{
    _count_of_node++;
//    for(size_t i = 0, end = pointers.size(); i < end; ++i)
//    {
//        pointers[i] = 0;
//    }
    pointers.push_back(0);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node() : _is_leaf(true), size(0), position_in_disk(_count_of_node)
{
    _count_of_node++;
//    for(size_t i = 0, end = pointers.size(); i < end; ++i)
//    {
//        pointers[i] = 0;
//    }
    pointers.push_back(0);
}

//template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
//logger *B_tree_disk<tkey, tvalue, compare, t>::get_logger() const noexcept
//{
//    return _logger;
//}

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
B_tree_disk<tkey, tvalue, compare, t>::B_tree_disk(const std::string& file_path, const compare& cmp, void* logger):  compare(cmp)
{
//    std::cout<<"constructor tree start" <<std::endl;
//
//    _file_for_tree.clear();
//    _file_for_tree.open(file_path, std::ios::binary | std::ios::out);
//    _file_for_tree.close();
//    _file_for_tree.open(file_path, std::ios::binary | std::ios::in | std::ios::out);
//
//    if (!_file_for_tree.is_open()) {
//        _file_for_tree.clear();
//        _file_for_tree.open(file_path, std::ios::binary | std::ios::out);
//        _file_for_tree.close();
//        _file_for_tree.open(file_path, std::ios::binary | std::ios::in | std::ios::out);
//
//    }
//
//    if (!_file_for_tree.is_open()) {
//        throw std::runtime_error("Couldn't open file " + file_path);
//    }
//
//    size_t last_index = file_path.find_last_of('.');
//    std::string raw_name = file_path.substr(0, last_index);
//    std::string filename_for_file_key_value = raw_name + "_data.bin";
//
//    _file_for_key_value.clear();
//    _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::out);
//    _file_for_key_value.close();
//    _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::in | std::ios::out);
//
//
//    if (!_file_for_key_value.is_open()) {
//        _file_for_key_value.clear();
//        _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::out);
//        _file_for_key_value.close();
//        _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::in | std::ios::out);
//    }
//
//    if (!_file_for_key_value.is_open()) {
//        throw std::runtime_error("Couldn't open file " + filename_for_file_key_value);
//    }
//
//    _count_of_node = 1;
//
//    _root = btree_disk_node(true);
//
//    _position_root = 1;
//
//    print_root_position();
//
//    disk_write(_root);
//
//
//    _file_for_tree.clear();
//    _file_for_tree.open(file_path, std::ios::binary | std::ios::out);
//    _file_for_tree.close();



    if(std::filesystem::exists(file_path))
    {
        _file_for_tree.open(file_path, std::ios_base::binary | std::ios_base::in |std::ios_base::out);


        size_t last_index = file_path.find_last_of('.');

        std::string raw_name = file_path.substr(0, last_index);

        std::string filename_for_file_key_value = raw_name + "_data.bin";

        _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::in | std::ios::out);


        _file_for_tree.seekg(sizeof(size_t), std::ios::beg);

        _file_for_tree.read(reinterpret_cast<char*>(&_position_root), sizeof(size_t));

        std::cout << _position_root << std::endl;

        _file_for_tree.seekg(0, std::ios::end);

        size_t last_pos = _file_for_tree.tellg();

        size_t amount_for_one_node = 3 * sizeof(size_t) + sizeof(bool) + (2 * sizeof(size_t)) * (maximum_keys_in_node + 1) + sizeof(size_t) * (maximum_keys_in_node + 2);

        _count_of_node = last_pos / amount_for_one_node;

        if (_count_of_node * amount_for_one_node != last_pos)
            ++_count_of_node;

    }
    else
    {
        _file_for_tree.clear();
        _file_for_tree.open(file_path, std::ios::binary | std::ios::out);
        _file_for_tree.close();
        _file_for_tree.open(file_path, std::ios::binary | std::ios::in | std::ios::out);

        if (!_file_for_tree.is_open()) {
            _file_for_tree.clear();
            _file_for_tree.open(file_path, std::ios::binary | std::ios::out);
            _file_for_tree.close();
            _file_for_tree.open(file_path, std::ios::binary | std::ios::in | std::ios::out);

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

        btree_disk_node root = btree_disk_node(true);

        _position_root = 1;

        print_root_position();

        disk_write(root);


//        _file_for_tree.clear();
//        _file_for_tree.open(file_path, std::ios::binary | std::ios::out);
//        _file_for_tree.close();

//        _file_for_tree.clear();
//        _file_for_tree.open(file_path, std::ios::binary | std::ios::out);
//        _file_for_tree.close();
//        _file_for_tree.open(file_path, std::ios::binary | std::ios::in | std::ios::out);
//
////        if (!_file_for_tree.is_open()) {
////            _file_for_tree.clear();
////            _file_for_tree.open(file_path, std::ios::binary | std::ios::out);
////            _file_for_tree.close();
////            _file_for_tree.open(file_path, std::ios::binary | std::ios::in | std::ios::out);
////        }
////
////        if (!_file_for_tree.is_open()) {
////            throw std::runtime_error("Couldn't open file " + file_path);
////        }
//
//        size_t last_index = file_path.find_last_of('.');
//        std::string raw_name = file_path.substr(0, last_index);
//        std::string filename_for_file_key_value = raw_name + "_data.bin";
//
////        _file_for_key_value.clear();
////        _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::out);
////        _file_for_key_value.close();
////        _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::in | std::ios::out);
//
//        _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::in | std::ios::out);
//
////        if (!_file_for_key_value.is_open()) {
////            _file_for_key_value.clear();
////            _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::out);
////            _file_for_key_value.close();
////            _file_for_key_value.open(filename_for_file_key_value, std::ios::binary | std::ios::in | std::ios::out);
////        }
////
////        if (!_file_for_key_value.is_open()) {
////            throw std::runtime_error("Couldn't open file " + filename_for_file_key_value);
////        }
//
//        _count_of_node = 1;
//
//        _root = btree_disk_node(true);
//
//        _position_root = 1;
//
//        print_root_position();
//
//        disk_write(_root);

    }

}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::check_tree(size_t pos, size_t depth)
{
    if (pos == 0)
        return;
    btree_disk_node node = disk_read(pos);
    if(node.size == 0)
    {
        std::cout << "empty" << std::endl;
        return;
    }
    if (!node._is_leaf)
    {
        check_tree(node.pointers[0], depth + 1);
    }
    std::string tab;
    tab.reserve(depth);
    for (size_t j = 0; j < depth; ++j)
    {
        tab += '\t';
    }
    std::cout << tab << std::to_string(node.keys[0].first._data)<< " " << std::to_string(node.keys[0].second._data) << std::endl;
    for(size_t i = 1; i < node.size; ++i)
    {
//        if (!search_tree<tkey, tvalue>::_keys_comparer((node->_pairs_of_node[i - 1]).first, (node->_pairs_of_node[i]).first))
//            throw std::logic_error("B");
        if (!node._is_leaf)
        {
            check_tree(node.pointers[i], depth + 1);
        }
        std::cout << tab << std::to_string(node.keys[i].first._data) << " " << std::to_string(node.keys[i].second._data) <<std::endl;
    }

    if (!node._is_leaf)
    {
        check_tree(node.pointers[node.size], depth + 1);
    }

    if (depth == 0)
        std::cout << std::endl << std:: endl;
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::btree_disk_const_iterator(B_tree_disk<tkey, tvalue, compare, t>& tree, const std::stack<std::pair<size_t, size_t>> &path, size_t index) : _tree(tree), _path(path), _index(index) {}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator B_tree_disk<tkey, tvalue, compare, t>::begin()
{
    btree_disk_node root = disk_read(_position_root);

    if(root.size == 0)
    {
        return btree_disk_const_iterator(*this);
    }

    std::stack<std::pair<size_t, size_t>> stk;
    stk.push(std::make_pair(_position_root, size_t(0)));

    btree_disk_node node = disk_read(stk.top().first);

    while(node.size > 0 && node.pointers[0] != 0)
    {
        stk.push(std::make_pair(node.pointers[0], size_t(0)));
        node = disk_read(stk.top().first);
    }

    return btree_disk_const_iterator(*this, stk, 0);

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator B_tree_disk<tkey, tvalue, compare, t>::end()
{
    return btree_disk_const_iterator(*this);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self& B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator++()
{
    if(_path.empty())
    {
        return *this;
    }

    btree_disk_node node = _tree.disk_read(_path.top().first);

    if(node._is_leaf)
    {
        ++_index;

        while(!_path.empty() && _index == node.size)
        {

            _index = _path.top().second;
            _path.pop();
            if(!_path.empty())
                node = _tree.disk_read(_path.top().first);
        }
    }
    else
    {
        _path.push(std::make_pair(node.pointers[_index + 1], _index + 1));
        _index = 0;

        node = _tree.disk_read(_path.top().first);
        while (!node._is_leaf)
        {
            _path.push(std::make_pair(node.pointers[_index], _index));
            _index = 0;
            node = _tree.disk_read(_path.top().first);
        }
    }

    return *this;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self& B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator--()
{
    if(_path.empty())
    {
        return *this;
    }

    btree_disk_node node = _tree.disk_read(_path.top().first);

    if(node._is_leaf)
    {
        if(_index == 0)
        {
            while(!_path.empty() && _index == node.size)
            {

                _index = _path.top().second;
                _path.pop();
                if(!_path.empty())
                    node = _tree.disk_read(_path.top().first);
            }
        }

        --_index;
    }
    else
    {
//        _path.push(std::make_pair(node.pointers[_index + 1], _index + 1));
//        _index = 0;

        node = _tree.disk_read(_path.top().first);
        while (!node._is_leaf)
        {
            _path.push(std::make_pair(node.pointers[_index], _index));
            _index = node.size;
            node = _tree.disk_read(_path.top().first);
        }
        _index = node.size - 1;
    }

    return *this;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator--(int)
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator==(B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self &other) noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top().first == other._path.top().first && _index == other._index);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator!=(B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self &other) noexcept
{
    return !(*this == other);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::value_type B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator*() noexcept
{
    btree_disk_node node = _tree.disk_read(_path.top().first);
    return *reinterpret_cast<tree_data_type_const*>(&(node.keys[_index]));
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::optional<tvalue> B_tree_disk<tkey, tvalue, compare, t>::at(const tkey & key)
{
    auto path = find_path(key);
    auto path_path = path.first;

    if(path.second.second == false)
    {
        return std::optional<tvalue>();
    }

    btree_disk_node current_node = disk_read(path_path.top().first);

    auto [ind, found] = find_index(key, current_node);

    if(!found)
    {
        return std::optional<tvalue>();
    }

    return current_node.keys[ind].second;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator, typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator> B_tree_disk<tkey, tvalue, compare, t>::find_range(const tkey &lower, const tkey &upper, bool include_lower, bool include_upper)
{
    if (compare_keys(upper, lower))
    {
        throw std::logic_error("Upper key is less then lower");
    }

    btree_disk_node node = disk_read(_position_root);

    if (node.size == 0) {
        return std::make_pair(end(), end());
    }
    auto [ptr, ind] = find_path(lower);

    btree_disk_const_iterator beg(*this, ptr, ind.first);

    node = disk_read(ptr.top().first);
    auto [in_ind, found] = find_index(lower, node);

    if (in_ind > node.size - 1)
        in_ind = node.size - 1;

    beg._index = in_ind;

    auto it_end = end();
    while (beg != it_end && compare_keys((*beg).first, lower))
    {
        ++beg;
    }

    if (beg == it_end)
        return std::make_pair(end(), end());

    bool found_one = false;

    if (!compare_keys(lower, (*beg).first))
    {
        if (!include_lower)
            ++beg;
        else
            found_one = true;
    }

    auto [ptr_end, ind_end] = find_path(upper);

    btree_disk_const_iterator e(*this, ptr_end, ind_end.first);

    node = disk_read(ptr.top().first);
    auto [in_ind2, found2] = find_index(upper, node);

    if (in_ind2 > node.size - 1)
    {
        in_ind2 = node.size - 1;
        e._index = in_ind2;
        ++e;
    } else {
        e._index = in_ind2;
    }


    while (beg != e && e != it_end && compare_keys(upper, (*e).first))
    {
        --e;
    }

    if (e != it_end && !compare_keys((*e).first, upper))
    {
        if (!include_upper)
        {
            if (beg != e)
                --e;
        } else
        {
            found_one = true;
        }
    }

    if (beg != e)
        found_one = true;

    return std::make_pair(beg, found_one ? ++e : e);
}



#endif //B_DISK_TREE_B_TREE_DISK_H
