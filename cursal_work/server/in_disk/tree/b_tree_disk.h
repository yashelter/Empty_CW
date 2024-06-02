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

#include "../../interfaces/controller_int.h"
#include "../../interfaces/operation.h"


template <serializable tkey, serializable tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
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

    struct btree_disk_node
    {
        size_t size;
        std::array<tree_data_type, maximum_keys_in_node + 1> keys;
        std::array<btree_disk_node*, maximum_keys_in_node + 2> pointers;

        void serialize(std::fstream& stream) const;
        static btree_disk_node deserialize(std::fstream& stream);

        btree_disk_node() noexcept;
    };

    logger* _logger;

    mutable std::fstream file;

    logger* get_logger() const noexcept override;

public:

    // region constructors declaration

    explicit B_tree_disk(const std::string& file_path, const compare& cmp = compare(), logger* logger = nullptr);


    // endregion constructors declaration

    // region five declaration

    B_tree_disk(B_tree_disk&& other) noexcept =default;
    B_tree_disk& operator=(B_tree_disk&& other) noexcept =default;

    B_tree_disk(const B_tree_disk& other) =delete;
    B_tree_disk& operator=(const B_tree_disk& other) =delete;

    ~B_tree_disk() noexcept =default;

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

private:

};

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

#endif //MP_OS_B_TREE_DISK_H
