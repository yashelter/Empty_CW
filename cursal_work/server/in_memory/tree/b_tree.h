//
// Created by Des Caldnd on 4/12/2024.
//

#include <iterator>
#include <utility>
#include <array>
#include <concepts>
#include <stack>
#include <initializer_list>
#include <allocator_guardant.h>
#include <logger_guardant.h>
#include "../../interfaces/controller_int.h"
#include "../../interfaces/operation.h"
#include <shared_mutex>

#ifndef CW_B_TREE_H
#define CW_B_TREE_H

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class B_tree final : public allocator_guardant, public logger_guardant, private compare
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

    struct btree_node
    {
		size_t size;
        std::array<tree_data_type, maximum_keys_in_node + 1> keys;
        std::array<btree_node*, maximum_keys_in_node + 2> pointers;

		btree_node() noexcept;
    };

    allocator* _allocator;
    logger* _logger;

    mutable btree_node* _root;

    size_t _size;

    logger* get_logger() const noexcept override;
    allocator* get_allocator() const noexcept override;

    class b_tree_operation : public operation<B_tree<tkey, tvalue, compare, t>>{};

    class b_tree_insert_operation : public b_tree_operation
    {
        tree_data_type _data;
        void undo(B_tree& tree) override;
        void redo(B_tree& tree) override;
    };

    class b_tree_update_operation : public b_tree_operation
    {
        tree_data_type _data;
        void undo(B_tree& tree) override;
        void redo(B_tree& tree) override;
    };

    class b_tree_remove_operation : public b_tree_operation
    {
        tree_data_type _data;
        void undo(B_tree& tree) override;
        void redo(B_tree& tree) override;
    };

    using time_point_t = controller_int<tkey, tvalue>::time_point_t;

    std::stack<std::pair<time_point_t, std::shared_ptr<b_tree_operation>>> _operations;
    std::stack<std::pair<time_point_t, std::shared_ptr<b_tree_operation>>> _undone_operations;

public:

    void revert_to(time_point_t time);
    void redo_all();

    std::shared_mutex _mut;

    // region constructors declaration

    explicit B_tree(const compare& cmp = compare(), allocator* allocator = nullptr, logger* logger = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit B_tree(iterator begin, iterator end, const compare& cmp = compare(), allocator* allocator = nullptr, logger* logger = nullptr);


	B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), allocator* allocator = nullptr, logger* logger = nullptr);

    // endregion constructors declaration

    // region five declaration

    B_tree(const B_tree& other);
    B_tree(B_tree&& other) noexcept;

    B_tree& operator=(const B_tree& other);
    B_tree& operator=(B_tree&& other) noexcept;

    ~B_tree() noexcept override;

    // endregion five declaration

    // region iterators declaration

    class btree_iterator;
    class btree_reverse_iterator;
    class btree_const_iterator;
    class btree_const_reverse_iterator;

    class btree_iterator
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;

        using self = btree_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;

        size_t current_node_keys_count() const noexcept;

        bool is_terminate_node() const noexcept;

        explicit btree_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_const_iterator
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;
    public:
        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;

        using self = btree_const_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_iterator;
        friend class btree_const_reverse_iterator;

        btree_const_iterator(const btree_iterator& it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;

        size_t current_node_keys_count() const noexcept;

        bool is_terminate_node() const noexcept;

        explicit btree_const_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_reverse_iterator
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;
    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;

        using self = btree_reverse_iterator;

        friend class B_tree;
        friend class btree_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        btree_reverse_iterator(const btree_iterator& it) noexcept;
        operator btree_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;

        size_t current_node_keys_count() const noexcept;

        bool is_terminate_node() const noexcept;

        explicit btree_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_const_reverse_iterator
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;
    public:
        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;

        using self = btree_const_reverse_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_iterator;

        btree_const_reverse_iterator(const btree_reverse_iterator& it) noexcept;
        operator btree_const_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;

        size_t current_node_keys_count() const noexcept;

        bool is_terminate_node() const noexcept;

        explicit btree_const_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;
    friend class btree_reverse_iterator;
    friend class btree_const_reverse_iterator;

    // endregion iterators declaration

    // region element access declaration

    /*
     * Returns a reference to the mapped value of the element with specified key. If no such element exists, an exception of type std::out_of_range is thrown.
     */
    tvalue& at(const tkey&);
    const tvalue& at(const tkey&) const;


    /*
     * If key not exists, makes default initialization of value
     */
    tvalue& operator[](const tkey& key);
    tvalue& operator[](tkey&& key);

    // endregion element access declaration

    // region iterator begins declaration

    btree_iterator begin();
    btree_iterator end();

    btree_const_iterator begin() const;
    btree_const_iterator end() const;

    btree_const_iterator cbegin() const;
    btree_const_iterator cend() const;

    btree_reverse_iterator rbegin();
    btree_reverse_iterator rend();

    btree_const_reverse_iterator rbegin() const;
    btree_const_reverse_iterator rend() const;

    btree_const_reverse_iterator crbegin() const;
    btree_const_reverse_iterator crend() const;

    // endregion iterator begins declaration

    // region lookup declaration

    size_t size() const noexcept;

	bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */
    btree_iterator find(const tkey& key);
    btree_const_iterator find(const tkey& key) const;

    std::pair<btree_iterator, btree_iterator> find_range(const tkey& lower, const tkey& upper, bool include_lower = true, bool include_upper = false);

	std::pair<btree_const_iterator, btree_const_iterator> find_range(const tkey& lower, const tkey& upper, bool include_lower = true, bool include_upper = false) const;

    bool contains(const tkey& key) const;

    // endregion lookup declaration

    // region modifiers declaration

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<btree_iterator, bool> insert(const tree_data_type& data);
    std::pair<btree_iterator, bool> insert(tree_data_type&& data);

    bool update(const tree_data_type& data);
    bool update(tree_data_type&& data);

    template <typename ...Args>
    std::pair<btree_iterator, bool> emplace(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    btree_iterator erase(btree_iterator pos);
    btree_iterator erase(const tkey& key);

    // endregion modifiers declaration

private:

    btree_node* copy_subtree(btree_node* parent, btree_node* copyable);

    void destroy_subtree(btree_node* node) noexcept;

    btree_iterator erase_inner(std::stack<std::pair<btree_node**, size_t>>& path, size_t index) noexcept;

    std::pair<std::stack<std::pair<btree_node**, size_t>>, size_t> find_path(const tkey& key) const noexcept;

	std::pair<size_t, bool> find_index(const tkey& key, btree_node* node) const noexcept;

    static bool exists(btree_node* node, size_t ind) noexcept;

	static bool is_terminate_node(btree_node* node) noexcept;

    btree_iterator emplace_inner(tree_data_type&& data, std::stack<std::pair<btree_node**, size_t>>& path);

	static void insert_array(btree_node* node, btree_node* right_node, tree_data_type&& data, size_t index) noexcept;

	static btree_node* remove_array(btree_node* node, size_t index, bool remove_left_ptr = true) noexcept;

	void split_node(std::stack<std::pair<btree_node**, size_t>>& path, btree_node*& node, size_t& index);

	void rebalance_node(std::stack<std::pair<btree_node**, size_t>>& path, btree_node*& node, size_t& index);
};

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::rebalance_node(std::stack<std::pair<btree_node * *, size_t>> &path,
                                                      B_tree::btree_node *&node,
                                                      size_t &index)
{
	if (path.empty())
		return;
	if (path.size() == 1)
	{
		if ((*path.top().first)->size == 0)
		{
			allocator::destruct(*path.top().first);
			deallocate_with_guard(*path.top().first);
			*path.top().first = nullptr;
			node = nullptr;
		}
		path.pop();
		return;
	} else
	{
		auto pair = path.top();
		auto current_node = *pair.first;
		path.pop();

		bool need_key = true;

		if (pair.second > 0)
		{
			btree_node* brother = (*path.top().first)->pointers[pair.second - 1];

			if (brother->size > minimum_keys_in_node)
			{
				auto tmp = (*path.top().first)->keys[pair.second - 1];
				std::swap((*path.top().first)->keys[pair.second - 1], brother->keys[brother->size - 1]);

				auto tmp_ptr = remove_array(brother, brother->size - 1, false);

                std::swap(current_node->pointers[0], tmp_ptr);

				insert_array(current_node, tmp_ptr, std::move(tmp), 0);

				++index;

				need_key = false;
			}
		}

		if (need_key && pair.second < (*path.top().first)->size)
		{
			btree_node* brother = (*path.top().first)->pointers[pair.second + 1];

			if (brother->size > minimum_keys_in_node)
			{
				auto tmp = (*path.top().first)->keys[pair.second];
				std::swap((*path.top().first)->keys[pair.second], brother->keys[0]);

				auto tmp_ptr = remove_array(brother, 0);

				insert_array(current_node, tmp_ptr, std::move(tmp), current_node->size);

				need_key = false;
			}
		}

		if (need_key)
		{
			btree_node* parent = *path.top().first;
			btree_node* left = pair.second > 0 ? parent->pointers[pair.second - 1] : current_node;
			btree_node* right = pair.second > 0 ? current_node : parent->pointers[pair.second + 1];
			size_t parent_index = pair.second > 0 ? pair.second - 1 : pair.second;
			if (pair.second > 0)
			{
				node = left;
				index += left->size + 1;
			}

			auto tmp_data = parent->keys[parent_index];

			insert_array(left, right->pointers[0], std::move(tmp_data), left->size);

			for (size_t i = 0; i < right->size; ++i)
			{
				insert_array(left, right->pointers[i + 1], std::move(right->keys[i]), left->size);
			}

			remove_array(parent, parent_index, false);

			allocator::destruct(right);
			deallocate_with_guard(right);

			if (path.size() == 1 && (*path.top().first)->size == 0)
			{
				path.pop();
				allocator::destruct(parent);
				deallocate_with_guard(parent);
				_root = left;
			}
		}
	}
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::erase_inner(std::stack<std::pair<btree_node * *, size_t>> &path,
                                                                                                        size_t index) noexcept
{
	--_size;
	if (!is_terminate_node(*path.top().first))
	{
		btree_node* tmp = *path.top().first;

		path.push(std::make_pair(&(*path.top().first)->pointers[index], index));

		while (!is_terminate_node(*path.top().first))
		{
			path.push(std::make_pair(&(*path.top().first)->pointers[(*path.top().first)->size], (*path.top().first)->size));
		}

		std::swap(tmp->keys[index], (*path.top().first)->keys[(*path.top().first)->size - 1]);
		index = (*path.top().first)->size - 1;
	}

	remove_array(*path.top().first, index);

	btree_node* n = *path.top().first;

	while (!path.empty() && (*path.top().first)->size < minimum_keys_in_node)
	{
		rebalance_node(path, n, index);
	}

	if (n == nullptr)
		return end();
	auto res = find_path(n->keys[index].first);

//    check_tree(_root);

	return btree_iterator(res.first, res.second);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::split_node(std::stack<std::pair<btree_node * *, size_t>>& path, btree_node*& node, size_t& index)
{
	auto new_node = reinterpret_cast<btree_node*>(allocate_with_guard(sizeof(btree_node)));
	allocator::construct(new_node);

	if (path.size() == 1)
	{
		try
		{
			auto tmp = reinterpret_cast<btree_node*>(allocate_with_guard(sizeof(btree_node)));
			allocator::construct(tmp);


			_root = std::exchange(tmp, _root);

			_root->pointers[0] = tmp;

            path.pop();

            path.push(std::make_pair(&_root, 0));
            path.push(std::make_pair(&(*path.top().first)->pointers[0], size_t(0)));

		}catch (...)
		{
			allocator::destruct(new_node);
			deallocate_with_guard(new_node);
			throw;
		}
	}

	size_t separator = (*path.top().first)->size / 2;

    auto pair = path.top();
	auto node_ptr = *pair.first;
    path.pop();

	insert_array(*path.top().first, new_node, std::move(node_ptr->keys[separator - 1]), pair.second);

	new_node->pointers[0] = is_terminate_node(node_ptr) ? nullptr : node_ptr->pointers[separator];
    new_node->size = node_ptr->size - separator;

	for (size_t i = separator; i < node_ptr->size; ++i)
	{
		allocator::construct(std::addressof(new_node->keys[i - separator]), std::move(node_ptr->keys[i]));
		new_node->pointers[i - separator + 1] = node_ptr->pointers[i + 1];
	}

	if (is_terminate_node(node_ptr))
		new_node->pointers[0] = nullptr;

	node_ptr->size = separator - 1;

    if (node == node_ptr)
    {
        if (index == separator - 1)
        {
            node = *path.top().first;
            index = pair.second;
        } else if (index >= separator)
        {
            node = new_node;
            index -= separator;
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_node::btree_node() noexcept : size(0)
{
	pointers[0] = nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::insert_array(btree_node* node, btree_node* right_node, tree_data_type&& data, size_t index) noexcept
{
	for (size_t i = node->size; i > index; --i)
	{
		allocator::construct(std::addressof(node->keys[i]), std::move(node->keys[i - 1]));
		allocator::destruct(std::addressof(node->keys[i - 1]));
		node->pointers[i + 1] = node->pointers[i];
	}

	allocator::construct(std::addressof(node->keys[index]), std::move(data));
	node->pointers[index + 1] = right_node;
	++node->size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_node* B_tree<tkey, tvalue, compare, t>::remove_array(btree_node* node, size_t index, bool remove_left_ptr) noexcept
{
	btree_node* res = remove_left_ptr ? node->pointers[index] : node->pointers[index + 1];

	bool is_left_terminate = is_terminate_node(node);

    if (is_left_terminate && index == 0 && remove_left_ptr)
    {
        res = nullptr;
    }

	for(size_t i = index; i < node->size - 1; ++i)
	{
		std::swap(node->keys[i], node->keys[i + 1]);
		if (remove_left_ptr)
			node->pointers[i] = node->pointers[i + 1];
		else
			node->pointers[i + 1] = node->pointers[i + 2];
	}

	if (remove_left_ptr)
	{
		node->pointers[node->size - 1] = node->pointers[node->size];
	}

	allocator::destruct(std::addressof(node->keys[node->size - 1]));

	--node->size;

	if (is_left_terminate)
		node->pointers[0] = nullptr;

	return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::emplace_inner(B_tree::tree_data_type &&data,
                                                                                                          std::stack<std::pair<btree_node**, size_t>>& path)
{
	++_size;
	if (_root == nullptr)
	{
		btree_node* new_node = reinterpret_cast<btree_node*>(allocate_with_guard(sizeof(btree_node)));

		allocator::construct(new_node);

		++new_node->size;
		new_node->keys[0] = std::move(data);
		_root = new_node;

		return begin();
	}

	auto [ind, found] = find_index(data.first, *path.top().first);
	if (found)
		return btree_iterator(path, ind);



	insert_array(*path.top().first, nullptr, std::move(data), ind);

    btree_node* node = *path.top().first;

	while(!path.empty() && (*path.top().first)->size > maximum_keys_in_node)
	{
		split_node(path, node, ind);
	}

    auto res = find_path(node->keys[ind].first);

//    check_tree(_root);

	return btree_iterator(res.first, res.second);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator, typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator> B_tree<tkey, tvalue, compare, t>::find_range(const tkey &lower,
                                                                                                                                                                                         const tkey &upper,
                                                                                                                                                                                         bool include_lower,
                                                                                                                                                                                         bool include_upper) const
{
	if (compare_keys(upper, lower))
	{
		throw std::logic_error("Upper key is less then lower");
	}
	if (empty())
	{
		return std::make_pair(end(), end());
	}
	auto [ptr, ind] = find_path(lower);

	btree_const_iterator beg(ptr, ind);

	if (!exists(ptr, ind))
	{
		auto [in_ind, found] = find_index(lower, ptr);
		if (in_ind > ptr->size - 1)
			in_ind = ptr->size - 1;

		beg._index = in_ind;
	}

	while (beg != end() && compare_keys(beg->first, lower))
	{
		++beg;
	}

	if (beg == end())
		return std::make_pair(end(), end());

	bool found_one = false;

	if (!compare_keys(lower, beg->first))
	{
		if (!include_lower)
			++beg;
		else
			found_one = true;
	}

	auto [ptr_end, ind_end] = find_path(upper);

	btree_const_iterator e(ptr_end, ind_end);

	if (!exists(ptr_end, ind_end))
	{
		auto [in_ind, found] = find_index(lower, ptr);
		if (in_ind > ptr->size - 1)
			in_ind = ptr->size - 1;

		e._index = in_ind;
	}

	while (beg != e && compare_keys(upper, e->first))
	{
		--e;
	}

	if (!compare_keys(e->first, upper))
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

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, typename B_tree<tkey, tvalue, compare, t>::btree_iterator> B_tree<tkey, tvalue, compare, t>::find_range(const tkey &lower,
                                                                                                                                                                             const tkey &upper,
                                                                                                                                                                             bool include_lower,
                                                                                                                                                                             bool include_upper)
{
	if (compare_keys(upper, lower))
	{
		throw std::logic_error("Upper key is less then lower");
	}
	if (empty())
		return std::make_pair(end(), end());
	auto [ptr, ind] = find_path(lower);

	btree_iterator beg(ptr, ind);

	if (!exists(ptr, ind))
	{
		auto [in_ind, found] = find_index(lower, ptr);
		if (in_ind > ptr->size - 1)
			in_ind = ptr->size - 1;

		beg._index = in_ind;
	}

	while (beg != end() && compare_keys(beg->first, lower))
	{
		++beg;
	}

	if (beg == end())
		return std::make_pair(end(), end());

	bool found_one = false;

	if (!compare_keys(lower, beg->first))
	{
		if (!include_lower)
			++beg;
		else
			found_one = true;
	}

	auto [ptr_end, ind_end] = find_path(upper);

	btree_iterator e(ptr_end, ind_end);

	if (!exists(ptr_end, ind_end))
	{
		auto [in_ind, found] = find_index(lower, ptr);
		if (in_ind > ptr->size - 1)
			in_ind = ptr->size - 1;

		e._index = in_ind;
	}

	while (beg != e && compare_keys(upper, e->first))
	{
		--e;
	}

	if (!compare_keys(e->first, upper))
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

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<size_t, bool> B_tree<tkey, tvalue, compare, t>::find_index(const tkey &key, B_tree::btree_node *node) const noexcept
{
	auto beg = node->keys.begin(), end = beg + node->size;

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
			return std::make_pair(std::distance(node->keys.begin(), mid), true);
		}
	}

	if (beg == node->keys.begin() + node->size)
		return std::make_pair(node->size, false);

	if (compare_keys(key, beg->first))
		return std::make_pair(std::distance(node->keys.begin(), beg), false);
	else
		return std::make_pair(std::distance(node->keys.begin(), beg) + 1, false);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::destroy_subtree(B_tree::btree_node *node) noexcept
{
	if (node == nullptr)
		return;

	if (!is_terminate_node(node))
		for (size_t i = 0; i < node->size + 1; ++i)
		{
			destroy_subtree(node->pointers[i]);
		}

	allocator::destruct(node);
	deallocate_with_guard(node);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_node *B_tree<tkey, tvalue, compare, t>::copy_subtree(B_tree::btree_node *parent,
                                                                                                      B_tree::btree_node *copyable)
{
	if (copyable == nullptr)
		return nullptr;
	btree_node* new_node = reinterpret_cast<btree_node*>(allocate_with_guard(sizeof(btree_node)));

	try
	{
		allocator::construct(new_node, *copyable);
	} catch (...)
	{
		deallocate_with_guard(new_node);
		throw;
	}

	size_t i = 0;

    if (is_terminate_node(copyable))
    {
        new_node->pointers[0] = nullptr;
    } else
	try
	{
		for (; i < copyable->size + 1; ++i)
		{
			new_node->pointers[i] = copy_subtree(new_node, copyable->pointers[i]);
		}
	} catch(...)
	{
		for(size_t j = 0; j < i; ++j)
		{
			destroy_subtree(new_node->pointers[j]);
		}
	}

	return new_node;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>>, size_t> B_tree<tkey, tvalue, compare, t>::find_path(const tkey &key) const noexcept
{
	if (_root == nullptr)
    {
        std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> stack;
        stack.push(std::make_pair(&_root, size_t(0)));
        return std::make_pair(std::move(stack), size_t(1));
    }
	else
	{
        std::stack<std::pair<btree_node**, size_t>> stack;

        stack.push(std::make_pair(&_root, size_t(0)));

		auto [ind, found] = find_index(key, *stack.top().first);

		while(!found && !is_terminate_node(*stack.top().first))
		{
            stack.push(std::make_pair(&(*stack.top().first)->pointers[ind], ind));
			auto tmp = find_index(key, *stack.top().first);
			ind = tmp.first;
			found = tmp.second;
		}

		return found ? std::make_pair(stack, ind) : std::make_pair(stack, (*stack.top().first)->size + 1);
	}
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::exists(B_tree::btree_node *node, size_t ind) noexcept
{
	return node != nullptr && ind < (node->size + 1);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::is_terminate_node(B_tree::btree_node *node) noexcept
{
	return node == nullptr || node->pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::self &B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--()
{
	if(_path.empty())
		return *this;
	if (is_terminate_node())
	{
		if (_index == 0)
		{
			while (!_path.empty() && _index == 0)
			{
				_index = _path.top().second;
                _path.pop();
			}
		}

		--_index;
	} else
	{
		while(!is_terminate_node())
		{
			_path.push(std::make_pair(&(*_path.top().first)->pointers[_index], _index));
			_index = (*_path.top().first)->size;
		}
		--_index;
	}

	return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::self &B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--()
{
    if(_path.empty())
        return *this;
    if (is_terminate_node())
    {
        if (_index == 0)
        {
            while (!_path.empty() && _index == 0)
            {
                _index = _path.top().second;
                _path.pop();
            }
        }

        --_index;
    } else
    {
        while(!is_terminate_node())
        {
			_path.push(std::make_pair(&(*_path.top().first)->pointers[_index], _index));
			_index = (*_path.top().first)->size;
        }
        --_index;
    }

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::self &B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++()
{
    if(_path.empty())
        return *this;
    if (is_terminate_node())
    {
        if (_index == 0)
        {
            while (!_path.empty() && _index == 0)
            {
                _index = _path.top().second;
                _path.pop();
            }
        }

        --_index;
    } else
    {
        while(!is_terminate_node())
        {
            _path.push(std::make_pair(&(*_path.top().first)->pointers[_index], _index));
            _index = (*_path.top().first)->size;
        }
        --_index;
    }

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::self &B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++()
{
    if(_path.empty())
        return *this;
    if (is_terminate_node())
    {
        if (_index == 0)
        {
            while (!_path.empty() && _index == 0)
            {
                _index = _path.top().second;
                _path.pop();
            }
        }

        --_index;
    } else
    {
        while(!is_terminate_node())
        {
			_path.push(std::make_pair(&(*_path.top().first)->pointers[_index], _index));
			_index = (*_path.top().first)->size;
        }
        --_index;
    }

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::self &B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++()
{
	if (_path.empty())
		return *this;
	if(is_terminate_node())
	{
		++_index;

		while (!_path.empty() && _index == (*_path.top().first)->size)
		{
			_index = _path.top().second;
            _path.pop();
		}
	} else
	{
		_path.push(std::make_pair(&(*_path.top().first)->pointers[_index + 1], _index + 1));
		_index = 0;
		while (!is_terminate_node())
		{
            _path.push(std::make_pair(&(*_path.top().first)->pointers[_index], _index));
			_index = 0;
		}
	}

	return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::self &B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++()
{
    if (_path.empty())
        return *this;
    if(is_terminate_node())
    {
        ++_index;

        while (!_path.empty() && _index == (*_path.top().first)->size)
        {
            _index = _path.top().second;
            _path.pop();
        }
    } else
    {
		_path.push(std::make_pair(&(*_path.top().first)->pointers[_index + 1], _index + 1));
		_index = 0;
		while (!is_terminate_node())
		{
			_path.push(std::make_pair(&(*_path.top().first)->pointers[_index], _index));
			_index = 0;
		}
    }

    return *this;
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::self &B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--()
{
    if (_path.empty())
        return *this;
    if(is_terminate_node())
    {
        ++_index;

        while (!_path.empty() && _index == (*_path.top().first)->size)
        {
            _index = _path.top().second;
            _path.pop();
        }
    } else
    {
		_path.push(std::make_pair(&(*_path.top().first)->pointers[_index + 1], _index + 1));
		_index = 0;
		while (!is_terminate_node())
		{
			_path.push(std::make_pair(&(*_path.top().first)->pointers[_index], _index));
			_index = 0;
		}
    }

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::self &B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--()
{
    if (_path.empty())
        return *this;
    if(is_terminate_node())
    {
        ++_index;

        while (!_path.empty() && _index == (*_path.top().first)->size)
        {
            _index = _path.top().second;
            _path.pop();
        }
    } else
    {
		_path.push(std::make_pair(&(*_path.top().first)->pointers[_index + 1], _index + 1));
		_index = 0;
		while (!is_terminate_node())
		{
			_path.push(std::make_pair(&(*_path.top().first)->pointers[_index], _index));
			_index = 0;
		}
    }

    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(const B_tree::btree_iterator &it) noexcept
	: _path(it._path), _index(it._index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare,
	   t>::btree_reverse_iterator::btree_reverse_iterator(const B_tree::btree_iterator &it) noexcept
	   : _path(it._path), _index(it._index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator btree_iterator() const noexcept
{
	return B_tree::btree_iterator(_path, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare,
	   t>::btree_const_reverse_iterator::btree_const_reverse_iterator(const B_tree::btree_reverse_iterator &it) noexcept
	   : _path(it._path), _index(it._index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator btree_const_iterator() const noexcept
{
	return B_tree::btree_const_iterator(_path, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::self B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int)
{
	auto tmp = *this;
	++(*this);
	return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::self B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++(int)
{
	auto tmp = *this;
	++(*this);
	return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::self B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int)
{
	auto tmp = *this;
	++(*this);
	return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::self B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int)
{
	auto tmp = *this;
	++(*this);
	return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::self B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int)
{
	auto tmp = *this;
	--(*this);
	return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::self B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--(int)
{
	auto tmp = *this;
	--(*this);
	return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::self B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int)
{
	auto tmp = *this;
	--(*this);
	return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::self B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int)
{
	auto tmp = *this;
	--(*this);
	return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare,
			t>::btree_iterator::operator==(const B_tree::btree_iterator::self &other) const noexcept
{
	return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && *_path.top().first == *other._path.top().first&& _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare,
			t>::btree_const_iterator::operator==(const B_tree::btree_const_iterator::self &other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && *_path.top().first == *other._path.top().first&& _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare,
			t>::btree_reverse_iterator::operator==(const B_tree::btree_reverse_iterator::self &other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && *_path.top().first == *other._path.top().first&& _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare,
			t>::btree_const_reverse_iterator::operator==(const B_tree::btree_const_reverse_iterator::self &other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && *_path.top().first == *other._path.top().first&& _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare,
			t>::btree_iterator::operator!=(const B_tree::btree_iterator::self &other) const noexcept
{
	return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare,
			t>::btree_const_iterator::operator!=(const B_tree::btree_const_iterator::self &other) const noexcept
{
	return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare,
			t>::btree_reverse_iterator::operator!=(const B_tree::btree_reverse_iterator::self &other) const noexcept
{
	return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare,
			t>::btree_const_reverse_iterator::operator!=(const B_tree::btree_const_reverse_iterator::self &other) const noexcept
{
	return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept
{
	return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept
{
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept
{
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept
{
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept
{
    return (*_path.top().first)->size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept
{
    return (*_path.top().first)->size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept
{
    return (*_path.top().first)->size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept
{
    return (*_path.top().first)->size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept
{
    return _path.empty() || (*_path.top().first)->size == 0 || (*_path.top().first)->pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept
{
    return _path.empty() || (*_path.top().first)->size == 0 || (*_path.top().first)->pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept
{
    return _path.empty() || (*_path.top().first)->size == 0 || (*_path.top().first)->pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept
{
    return _path.empty() || (*_path.top().first)->size == 0 || (*_path.top().first)->pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_iterator::btree_iterator(const std::stack<std::pair<btree_node**, size_t>>& path, size_t index) : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(const std::stack<std::pair<btree_node**, size_t>>& path, size_t index) : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path, size_t index) : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path, size_t index) : _path(path), _index(index) {}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept
{
    return reinterpret_cast<tree_data_type_const*>(&(*_path.top().first)->keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept
{
    return *reinterpret_cast<tree_data_type_const*>(&(*_path.top().first)->keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept
{
    return reinterpret_cast<tree_data_type_const*>(&(*_path.top().first)->keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept
{
    return *reinterpret_cast<tree_data_type_const*>(&(*_path.top().first)->keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept
{
    return reinterpret_cast<tree_data_type_const*>(&(*_path.top().first)->keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept
{
    return *reinterpret_cast<tree_data_type_const*>(&(*_path.top().first)->keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept
{
    return reinterpret_cast<tree_data_type_const*>(&(*_path.top().first)->keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept
{
	return *reinterpret_cast<tree_data_type_const*>(&(*_path.top().first)->keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const
{
    return btree_const_reverse_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const
{
    if (_root == nullptr)
        return btree_const_reverse_iterator();

    std::stack<std::pair<btree_node**, size_t>> stack;
    stack.push(std::make_pair(&_root, 0));

    while ((*stack.top().first)->size > 0 && (*stack.top().first)->pointers[0] != nullptr)
    {
        stack.push(std::make_pair(&(*stack.top().first)->pointers[(*stack.top().first)->size], (*stack.top().first)->size));
    }
    return btree_const_reverse_iterator(stack, (*stack.top().first)->size - 1);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const
{
    return btree_const_reverse_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const
{
    if (_root == nullptr)
        return btree_const_reverse_iterator();

    std::stack<std::pair<btree_node**, size_t>> stack;
    stack.push(std::make_pair(&_root, 0));

    while ((*stack.top().first)->size > 0 && (*stack.top().first)->pointers[0] != nullptr)
    {
        stack.push(std::make_pair(&(*stack.top().first)->pointers[(*stack.top().first)->size], (*stack.top().first)->size));
    }
    return btree_const_reverse_iterator(stack, (*stack.top().first)->size - 1);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()
{
    return btree_reverse_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()
{
    if (_root == nullptr)
        return btree_reverse_iterator();

    std::stack<std::pair<btree_node**, size_t>> stack;
    stack.push(std::make_pair(&_root, 0));

    while ((*stack.top().first)->size > 0 && (*stack.top().first)->pointers[0] != nullptr)
    {
        stack.push(std::make_pair(&(*stack.top().first)->pointers[(*stack.top().first)->size], (*stack.top().first)->size));
    }
    return btree_reverse_iterator(stack, (*stack.top().first)->size - 1);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const
{
    return btree_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const
{
    if (_root == nullptr)
        return btree_const_iterator();

    std::stack<std::pair<btree_node**, size_t>> stack;
    stack.push(std::make_pair(&_root, 0));

    while ((*stack.top().first)->size > 0 && (*stack.top().first)->pointers[0] != nullptr)
    {
        stack.push(std::make_pair(&(*stack.top().first)->pointers[0], 0));
    }
    return btree_const_iterator(stack, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const
{
    return btree_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_const_iterator B_tree<tkey, tvalue, compare, t>::begin() const
{
    if (_root == nullptr)
        return btree_const_iterator();

    std::stack<std::pair<btree_node**, size_t>> stack;
    stack.push(std::make_pair(&_root, 0));

    while ((*stack.top().first)->size > 0 && (*stack.top().first)->pointers[0] != nullptr)
    {
        stack.push(std::make_pair(&(*stack.top().first)->pointers[0], 0));
    }
    return btree_const_iterator(stack, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_iterator B_tree<tkey, tvalue, compare, t>::end()
{
    return btree_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_iterator B_tree<tkey, tvalue, compare, t>::begin()
{
    if (_root == nullptr)
        return btree_iterator();

    std::stack<std::pair<btree_node**, size_t>> stack;
    stack.push(std::make_pair(&_root, 0));

    while ((*stack.top().first)->size > 0 && (*stack.top().first)->pointers[0] != nullptr)
    {
        stack.push(std::make_pair(&(*stack.top().first)->pointers[0], 0));
    }
    return btree_iterator(stack, 0);
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool> B_tree<tkey, tvalue, compare, t>::emplace(Args &&... args)
{
    tree_data_type data(std::forward<Args>(args)...);
//	debug_with_guard("Insert " + std::to_string(data.first));
//    check_tree(_root);

    auto [stack, ind] = find_path(data.first);

    if (exists(*stack.top().first, ind))
        return std::make_pair(btree_iterator(stack, ind), false);
    return std::make_pair(emplace_inner(std::move(data), stack), true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue &B_tree<tkey, tvalue, compare, t>::operator[](const tkey &key)
{
	auto [stack, ind] = find_path(key);

	if (!exists(*stack.top().first, ind))
		return emplace_inner(std::make_pair(std::move(key), tvalue()), stack)->second;

	return (*stack.top().first)->keys[ind].second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue &B_tree<tkey, tvalue, compare, t>::operator[](tkey&& key)
{
    auto [stack, ind] = find_path(key);

    if (!exists(*stack.top().first, ind))
        return emplace_inner(std::make_pair(std::move(key), tvalue()), stack)->second;

    return (*stack.top().first)->keys[ind].second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue &B_tree<tkey, tvalue, compare, t>::at(const tkey &key) const
{
	auto [stack, ind] = find_path(key);

	if (!exists(*stack.top().first, ind))
		throw std::out_of_range("No such key");

	return (*stack.top().first)->keys[ind].second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue &B_tree<tkey, tvalue, compare, t>::at(const tkey &key)
{
    auto [stack, ind] = find_path(key);

    if (!exists(*stack.top().first, ind))
        throw std::out_of_range("No such key");

    return (*stack.top().first)->keys[ind].second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey &key) const
{
    auto [ptr, ind] = find_path(key);

    return exists(ptr, ind) ? btree_const_iterator(ptr, ind) : cend();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey &key)
{
    auto [ptr, ind] = find_path(key);

    return exists(ptr, ind) ? btree_iterator(ptr, ind) : end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::contains(const tkey &key) const
{
    auto [ptr, ind] = find_path(key);

    return exists(ptr, ind);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::empty() const noexcept
{
    return _size == 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::B_tree::btree_iterator B_tree<tkey, tvalue, compare, t>::erase(const tkey &key)
{
    auto [stack, ind] = find_path(key);

//	debug_with_guard("Erase " + std::to_string(key));
//    check_tree(_root);
	if (!exists(*stack.top().first, ind))
		return end();
    return erase_inner(stack, ind);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::erase(B_tree::btree_iterator pos)
{
    return erase_inner(pos._path, pos._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool> B_tree<tkey, tvalue, compare, t>::insert(const B_tree::tree_data_type &data)
{
    return emplace(data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool> B_tree<tkey, tvalue, compare, t>::insert(B_tree::tree_data_type&& data)
{
    return emplace(std::move(data));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t> &B_tree<tkey, tvalue, compare, t>::operator=(const B_tree &other)
{
    if (this != &other)
    {
		btree_node* tmp = copy_subtree(nullptr, other._root);
		destroy_subtree(_root);
		_root = tmp;
        compare::operator=(other);
        _size = other._size;
        _allocator = other._allocator;
        _logger = other._logger;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const B_tree &other) : compare(other), _allocator(other._allocator), _logger(other._logger), _size(other._size)
{
    _root = copy_subtree(nullptr, other._root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::~B_tree() noexcept
{
    destroy_subtree(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t> &B_tree<tkey, tvalue, compare, t>::operator=(B_tree &&other) noexcept
{
    if (this != &other)
    {
        std::swap(_allocator, other._allocator);
        std::swap(_logger, other._logger);
        std::swap(_size, other._size);
        std::swap(_root, other._root);
        compare::operator=(other);
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(B_tree &&other) noexcept : compare(other), _allocator(other._allocator), _logger(other._logger), _root(other._root), _size(other._size)
{
    other._root = nullptr;
    other._size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<input_iterator_for_pair<tkey, tvalue> iterator>
B_tree<tkey, tvalue, compare, t>::B_tree(iterator begin, iterator end, const compare &cmp, allocator *allocator,
                                              logger *logger) : compare(cmp), _allocator(allocator), _logger(logger), _root(nullptr), _size(0)
{
    for (; begin != end; ++begin)
    {
        emplace_or_assign(*begin);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp, allocator* allocator, logger* logger) : compare(cmp), _allocator(allocator), _logger(logger), _root(nullptr), _size(0)
{
    for (auto& val : data)
    {
        emplace_or_assign(val);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const compare &cmp, allocator *allocator, logger *logger) : compare(cmp), _allocator(allocator), _logger(logger), _root(nullptr), _size(0){}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger *B_tree<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return _logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
allocator *B_tree<tkey, tvalue, compare, t>::get_allocator() const noexcept
{
    return _allocator;
}

template<std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
    std::size_t t = 5>
B_tree(iterator begin, iterator end, const compare &cmp = compare(), allocator *allocator = nullptr,
            logger *logger = nullptr) -> B_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), allocator *allocator = nullptr,
            logger *logger = nullptr) -> B_tree<tkey, tvalue, compare, t>;


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_pairs(const B_tree::tree_data_type &lhs,
                                                     const B_tree::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}


#endif //CW_B_TREE_H
