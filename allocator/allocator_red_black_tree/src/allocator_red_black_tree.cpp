#include <not_implemented.h>

#include "../include/allocator_red_black_tree.h"

allocator_red_black_tree::~allocator_red_black_tree()
{
    get_mutex().~mutex();
    deallocate_with_guard(_trusted_memory);
}

allocator_red_black_tree::allocator_red_black_tree(
    allocator_red_black_tree &&other) noexcept
{
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
}

allocator_red_black_tree &allocator_red_black_tree::operator=(
    allocator_red_black_tree &&other) noexcept
{
    std::swap(_trusted_memory, other._trusted_memory);
    return *this;
}

allocator_red_black_tree::allocator_red_black_tree(
    size_t space_size,
    allocator *parent_allocator,
    logger *logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (space_size < free_block_metadata_size)
    {
        throw std::logic_error("To small space");
    }

    size_t real_size = space_size + allocator_metadata_size;

    _trusted_memory = (parent_allocator == nullptr) ? ::operator new(real_size) : parent_allocator->allocate(real_size, 1);

    auto logger_ptr = reinterpret_cast<class logger**>(_trusted_memory);

    *logger_ptr = logger;

    auto parent_allocator_ptr = reinterpret_cast<allocator**>(logger_ptr + 1);

    *parent_allocator_ptr = parent_allocator;

    auto fit_mode_ptr = reinterpret_cast<allocator_with_fit_mode::fit_mode*>(parent_allocator_ptr + 1);

    *fit_mode_ptr = allocate_fit_mode;

    auto size_ptr = reinterpret_cast<size_t*>(fit_mode_ptr + 1);

    *size_ptr = space_size;

    auto mutex_ptr = reinterpret_cast<std::mutex*>(size_ptr + 1);

    construct(mutex_ptr);

    auto first_block_ptr = reinterpret_cast<void**>(mutex_ptr + 1);

    *first_block_ptr = reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size;

    auto block_data_ptr = reinterpret_cast<void*>(reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size);

	get_data_from_block(block_data_ptr).color = block_color::BLACK;
	get_data_from_block(block_data_ptr).occupied = false;
	get_parent_from_block(block_data_ptr) = nullptr;
	get_back_from_block(block_data_ptr) = nullptr;
	get_forward_from_block(block_data_ptr) = nullptr;
	get_left_from_block(block_data_ptr) = nullptr;
	get_right_from_block(block_data_ptr) = nullptr;
}

[[nodiscard]] void *allocator_red_black_tree::allocate(
    size_t value_size,
    size_t values_count)
{
    std::lock_guard lock(get_mutex());

    size_t real_size = value_size * values_count;

    debug_with_guard("Allocator red black tree started allocating " + std::to_string(real_size) + " bytes");

    void* current_free;

    switch (get_fit_mod())
    {
        case allocator_with_fit_mode::fit_mode::first_fit:
            current_free = get_first(real_size);
            break;
        case allocator_with_fit_mode::fit_mode::the_best_fit:
            current_free = get_best(real_size);
            break;
        case allocator_with_fit_mode::fit_mode::the_worst_fit:
            current_free = get_worst(real_size);
            break;
    }

    if (current_free == nullptr)
    {
        error_with_guard("Allocator red black tree throwing bad_alloc while trying to allocate " + std::to_string(real_size) + " bytes");
        throw std::bad_alloc();
    }

    remove(current_free);

//	print_tree(*get_first_block_ptr(_trusted_memory));

    get_data_from_block(current_free).occupied = true;
	get_parent_from_block(current_free) = _trusted_memory;

    size_t free_block_size = get_block_size(current_free, _trusted_memory);

    bool need_fraq = true;

    if (free_block_size < real_size + free_block_metadata_size)
    {
        warning_with_guard("Allocator red black tree changed allocating block size to " + std::to_string(free_block_size));
        real_size = free_block_size;
        need_fraq = false;
    }

    if (need_fraq)
    {
        void* new_free_block = reinterpret_cast<std::byte*>(current_free) + occupied_block_metadata_size + real_size;

        get_forward_from_block(new_free_block) = get_forward_from_block(current_free);
        get_back_from_block(new_free_block) = current_free;

        get_forward_from_block(current_free) = new_free_block;
        if (get_forward_from_block(new_free_block) != nullptr)
            get_back_from_block(get_forward_from_block(new_free_block)) = new_free_block;

        get_data_from_block(new_free_block).occupied = false;
		get_parent_from_block(new_free_block) = nullptr;

        insert(new_free_block);
    }

    information_with_guard(std::to_string(get_free_size()));
    debug_with_guard(print_blocks());
//	print_tree(*get_first_block_ptr(_trusted_memory));

	debug_with_guard("Allocator red black tree finished allocating");
    return reinterpret_cast<std::byte*>(current_free) + occupied_block_metadata_size;
}

size_t allocator_red_black_tree::get_free_size() const noexcept
{
    size_t accum = 0;

    for (auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied())
            accum += it.size();
    }
    return accum;
}

void allocator_red_black_tree::deallocate(
    void *at)
{
	std::lock_guard lock(get_mutex());

	void* block_start = reinterpret_cast<std::byte*>(at) - occupied_block_metadata_size;

	if (get_parent_from_block(block_start) != _trusted_memory)
	{
		error_with_guard("Incorrect deallocation object");
		throw std::logic_error("Incorrect deallocation object");
	}

	size_t block_size = get_block_size(block_start, _trusted_memory);

	debug_with_guard("Allocator red black tree started deallocating " + std::to_string(block_size) + " bytes");

	if (block_size > 300)
		std::cout << 5;

	debug_with_guard(get_dump((char*)at, block_size));

	get_data_from_block(block_start).occupied == false;

	if(get_back_from_block(block_start) != nullptr && get_data_from_block(get_back_from_block(block_start)).occupied == false)
	{
		void* next = block_start;
		block_start = get_back_from_block(block_start);
		remove(block_start);

		get_forward_from_block(block_start) = get_forward_from_block(next);
		if(get_forward_from_block(block_start) != nullptr)
		{
			get_back_from_block(get_forward_from_block(block_start)) = block_start;
		}
	}

	if (get_forward_from_block(block_start) != nullptr && get_data_from_block(get_forward_from_block(block_start)).occupied == false)
	{
		void* next = get_forward_from_block(block_start);
		remove(next);

		get_forward_from_block(block_start) = get_forward_from_block(next);
		if (get_forward_from_block(block_start) != nullptr)
		{
			get_back_from_block(get_forward_from_block(block_start)) = block_start;
		}
	}

	insert(block_start);

//	print_tree(*get_first_block_ptr(_trusted_memory));

	information_with_guard(std::to_string(get_free_size()));
	debug_with_guard(print_blocks());
	debug_with_guard("Allocator red black tree finished deallocating");
}

void allocator_red_black_tree::set_fit_mode(allocator_with_fit_mode::fit_mode mode)
{
	std::lock_guard lock(get_mutex());
	get_fit_mod() = mode;
}

inline allocator *allocator_red_black_tree::get_allocator() const
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<allocator**>(byte_ptr + sizeof(logger*));
}

std::vector<allocator_test_utils::block_info> allocator_red_black_tree::get_blocks_info() const
{
    std::lock_guard lock(get_mutex());

    return get_blocks_info_inner();
}

inline logger *allocator_red_black_tree::get_logger() const
{
    return *reinterpret_cast<logger**>(_trusted_memory);
}

inline std::string allocator_red_black_tree::get_typename() const noexcept
{
    return "allocator_red_black_tree";
}

allocator_with_fit_mode::fit_mode &allocator_red_black_tree::get_fit_mod() const noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<fit_mode*>(byte_ptr + sizeof(logger*) + sizeof(allocator*));
}

size_t allocator_red_black_tree::get_overall_size(void *trusted_memory) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(trusted_memory);

    return *reinterpret_cast<size_t*>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode));
}

std::mutex &allocator_red_black_tree::get_mutex() const noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);
    return *reinterpret_cast<std::mutex*>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t));
}

void **allocator_red_black_tree::get_first_block_ptr(void *trusted_memory) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(trusted_memory);

    return reinterpret_cast<void**>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t) + sizeof(std::mutex));
}

std::vector<allocator_test_utils::block_info> allocator_red_black_tree::get_blocks_info_inner() const
{
    std::vector<allocator_test_utils::block_info> res;

    std::back_insert_iterator<std::vector<allocator_test_utils::block_info>> inserter(res);

    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        inserter = {it.size(), it.occupied()};
    }

    return res;
}

allocator_red_black_tree::rb_iterator allocator_red_black_tree::begin() const noexcept
{
    return {_trusted_memory};
}

allocator_red_black_tree::rb_iterator allocator_red_black_tree::end() const noexcept
{
    return {};
}

bool allocator_red_black_tree::is_block_occupied(void *block) noexcept
{
    return get_data_from_block(block).occupied;
}

size_t allocator_red_black_tree::get_block_size(void *block, void *trusted) noexcept
{
    if (get_forward_from_block(block) != nullptr)
        return reinterpret_cast<std::byte*>(get_forward_from_block(block)) - reinterpret_cast<std::byte*>(block) - occupied_block_metadata_size;
    else
        return reinterpret_cast<std::byte*>(trusted) + get_overall_size(trusted) + allocator_metadata_size - reinterpret_cast<std::byte*>(block) - occupied_block_metadata_size;
}

void *&allocator_red_black_tree::get_forward_from_block(void *block) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(block);

    return *reinterpret_cast<void**>(byte_ptr + sizeof(block_data) + sizeof(void*));
}

void*& allocator_red_black_tree::get_back_from_block(void* block) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(block);

    return *reinterpret_cast<void**>(byte_ptr + sizeof(block_data));
}

allocator_red_black_tree::block_data& allocator_red_black_tree::get_data_from_block(void* block) noexcept
{
    return *reinterpret_cast<block_data*>(block);
}

void*& allocator_red_black_tree::get_parent_from_block(void* block) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(block);

    return *reinterpret_cast<void**>(byte_ptr + sizeof(block_data) + 2 * sizeof(void*));
}

void*& allocator_red_black_tree::get_left_from_block(void* block) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(block);

    return *reinterpret_cast<void**>(byte_ptr + sizeof(block_data) + 3 * sizeof(void*));
}

void*& allocator_red_black_tree::get_right_from_block(void* block) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(block);

    return *reinterpret_cast<void**>(byte_ptr + sizeof(block_data) + 4 * sizeof(void*));
}

void allocator_red_black_tree::insert(void *block) noexcept
{
    void* current_top = *get_first_block_ptr(_trusted_memory);
    void* _parent = nullptr;

    while (current_top != nullptr)
    {
        if (get_block_size(block, _trusted_memory) < get_block_size(current_top, _trusted_memory))
        {
            _parent = current_top;
            current_top = get_left_from_block(current_top);
        } else
        {
            _parent = current_top;
            current_top = get_right_from_block(current_top);
        }
    }

    get_left_from_block(block) = nullptr;
    get_right_from_block(block) = nullptr;
    get_parent_from_block(block) = _parent;
    get_data_from_block(block).occupied = false;
    get_data_from_block(block).color = block_color::RED;

	if (_parent == nullptr)
	{
		*get_first_block_ptr(_trusted_memory) = block;
	} else
	{
		if (get_block_size(block, _trusted_memory) < get_block_size(_parent, _trusted_memory))
		{
			get_left_from_block(_parent) = block;
		} else
		{
			get_right_from_block(_parent) = block;
		}
	}

    bool need_continue = true;

    while(need_continue)
    {
        need_continue = false;

        if (get_parent_from_block(block) == nullptr)
        {
            get_data_from_block(block).color = block_color::BLACK;
        } else if (get_parent_from_block(block) != nullptr && get_parent_from_block(get_parent_from_block(block)) != nullptr &&
                            get_data_from_block(block).color == block_color::RED && get_data_from_block(get_parent_from_block(block)).color == block_color::RED)
        {
            void* child = block;
            void* parent = get_parent_from_block(block);
            void* grandparent = get_parent_from_block(parent);

            bool is_left_child = is_left_subtree(child, parent);
            bool is_left_parent = is_left_subtree(parent, grandparent);

            void* uncle = is_left_parent ? get_right_from_block(grandparent) : get_left_from_block(grandparent);

			block_color clr;
			if (uncle != nullptr)
				clr = get_data_from_block(uncle).color;

            if (uncle == nullptr)
            {
                if (is_left_parent)
                {
                    if (is_left_child)
                    {
                        small_right_rotation(grandparent);
                        get_data_from_block(child).color = block_color::RED;
                        get_data_from_block(parent).color = block_color::BLACK;
                        get_data_from_block(grandparent).color = block_color::RED;
                    } else
                    {
                        big_right_rotation(grandparent);
                        get_data_from_block(child).color = block_color::BLACK;
                        get_data_from_block(parent).color = block_color::RED;
                        get_data_from_block(grandparent).color = block_color::RED;
                    }
                } else
                {
                    if (!is_left_child)
                    {
                        small_left_rotation(grandparent);
                        get_data_from_block(child).color = block_color::RED;
                        get_data_from_block(parent).color = block_color::BLACK;
                        get_data_from_block(grandparent).color = block_color::RED;
                    } else
                    {
                        big_left_rotation(grandparent);
                        get_data_from_block(child).color = block_color::BLACK;
                        get_data_from_block(parent).color = block_color::RED;
                        get_data_from_block(grandparent).color = block_color::RED;
                    }
                }
            } else if (get_data_from_block(uncle).color == block_color::RED)
            {
                need_continue = true;

                get_data_from_block(uncle).color = block_color::BLACK;
                get_data_from_block(parent).color = block_color::BLACK;
                get_data_from_block(grandparent).color = block_color::RED;

                block = grandparent;
            } else
            {
                if (is_left_parent)
                {
                    if (is_left_child)
                    {
                        small_right_rotation(grandparent);
                        get_data_from_block(child).color = block_color::RED;
                        get_data_from_block(parent).color = block_color::BLACK;
                        get_data_from_block(grandparent).color = block_color::RED;
                    } else
                    {
                        big_right_rotation(grandparent);
                        get_data_from_block(child).color = block_color::BLACK;
                        get_data_from_block(parent).color = block_color::RED;
                        get_data_from_block(grandparent).color = block_color::RED;
                    }
                } else
                {
                    if (!is_left_child)
                    {
                        small_left_rotation(grandparent);
                        get_data_from_block(child).color = block_color::RED;
                        get_data_from_block(parent).color = block_color::BLACK;
                        get_data_from_block(grandparent).color = block_color::RED;
                    } else
                    {
                        big_left_rotation(grandparent);
                        get_data_from_block(child).color = block_color::BLACK;
                        get_data_from_block(parent).color = block_color::RED;
                        get_data_from_block(grandparent).color = block_color::RED;
                    }
                }
            }
        }
    }
}

void allocator_red_black_tree::remove(void *block) noexcept
{
    bool was_black_list = false;
    void* parent;

    if (get_left_from_block(block) == nullptr && get_right_from_block(block) == nullptr)
    {
        was_black_list = get_data_from_block(block).color == block_color::BLACK;
        parent = get_parent_from_block(block);
        reset_parent_ptr(block, nullptr);
		debug_with_guard("Remove zero");
    } else if (get_left_from_block(block) == nullptr || get_right_from_block(block) == nullptr)
    {
        void* node_of_interest = get_right_from_block(block) != nullptr ? get_right_from_block(block) : get_left_from_block(block);

        block_color color = get_data_from_block(block).color;

        if (color == block_color::BLACK)
        {
            get_data_from_block(node_of_interest).color = block_color::BLACK;
        }

        reset_parent_ptr(block, node_of_interest);
		get_parent_from_block(node_of_interest) = get_parent_from_block(block);

		debug_with_guard("Remove one");
    } else
    {
        void* node_of_interest = get_left_from_block(block);

        while (get_right_from_block(node_of_interest) != nullptr)
        {
            node_of_interest = get_right_from_block(node_of_interest);
        }

		debug_with_guard("Remove two");
        was_black_list = get_data_from_block(node_of_interest).color == block_color::BLACK && get_left_from_block(node_of_interest) == nullptr;

        parent = get_parent_from_block(node_of_interest);

        if (get_data_from_block(node_of_interest).color == block_color::BLACK && get_left_from_block(node_of_interest) != nullptr)
        {
			debug_with_guard("with one");

			get_data_from_block(get_left_from_block(node_of_interest)).color = block_color::BLACK;
        }

		reset_parent_ptr(block, node_of_interest);
		get_right_from_block(node_of_interest) = get_right_from_block(block);
		get_parent_from_block(get_right_from_block(node_of_interest)) = node_of_interest;

        if (get_parent_from_block(node_of_interest) != block)
        {
			reset_parent_ptr(node_of_interest, get_left_from_block(node_of_interest));
			if (get_left_from_block(node_of_interest) != nullptr)
				get_parent_from_block(get_left_from_block(node_of_interest)) = get_parent_from_block(node_of_interest);
			get_left_from_block(node_of_interest) = get_left_from_block(block);
			get_parent_from_block(get_left_from_block(node_of_interest)) = node_of_interest;
        } else
        {
            parent = node_of_interest;
        }
		get_data_from_block(node_of_interest).color = get_data_from_block(block).color;
		get_parent_from_block(node_of_interest) = get_parent_from_block(block);

    }

    if (was_black_list)
        rebalance_black_leaf(parent);
}

void allocator_red_black_tree::reset_parent_ptr(void *block, void *new_ptr)
{
    if (get_parent_from_block(block) == nullptr)
    {
        *get_first_block_ptr(_trusted_memory) = new_ptr;
    } else
    {
        if (is_left_subtree(block, get_parent_from_block(block)))
        {
            get_left_from_block(get_parent_from_block(block)) = new_ptr;
        } else
        {
            get_right_from_block(get_parent_from_block(block)) = new_ptr;
        }
    }
}

void allocator_red_black_tree::small_right_rotation(void *rotatable) noexcept
{
    if (get_left_from_block(rotatable) != nullptr)
    {
        void* node = get_left_from_block(rotatable);
        reset_parent_ptr(rotatable, node);

        get_parent_from_block(node) = get_parent_from_block(rotatable);

        get_parent_from_block(rotatable) = node;

        get_left_from_block(rotatable) = get_right_from_block(node);

        get_right_from_block(node) = rotatable;

		if (get_left_from_block(rotatable) != nullptr)
        	get_parent_from_block(get_left_from_block(rotatable)) = rotatable;
    }
}

void allocator_red_black_tree::small_left_rotation(void *rotatable) noexcept
{
    if (get_right_from_block(rotatable) != nullptr)
    {
        void* node = get_right_from_block(rotatable);
        reset_parent_ptr(rotatable, node);

        get_parent_from_block(node) = get_parent_from_block(rotatable);

        get_parent_from_block(rotatable) = node;

        get_right_from_block(rotatable) = get_left_from_block(node);

        get_left_from_block(node) = rotatable;

		if (get_right_from_block(rotatable) != nullptr)
        	get_parent_from_block(get_right_from_block(rotatable)) = rotatable;
    }
}

void allocator_red_black_tree::big_right_rotation(void *rotatable) noexcept
{
    if (get_left_from_block(rotatable) != nullptr && get_right_from_block(get_left_from_block(rotatable)) != nullptr)
    {
        void* node = get_left_from_block(rotatable);

        small_left_rotation(node);
        small_right_rotation(rotatable);
    }
}

void allocator_red_black_tree::big_left_rotation(void *rotatable) noexcept
{
    if (get_right_from_block(rotatable) != nullptr && get_left_from_block(get_right_from_block(rotatable)) != nullptr)
    {
        void* node = get_right_from_block(rotatable);

        small_right_rotation(node);
        small_left_rotation(rotatable);
    }
}

void allocator_red_black_tree::rebalance_black_leaf(void *parent, void *deleted)
{
	debug_with_guard("Rebalance");
    if(parent == nullptr)
    {
		if (deleted != nullptr)
			get_data_from_block(deleted).color = block_color::BLACK;
    } else
    {
        bool from_left = is_left_subtree(deleted, parent);

        void* brother = from_left ? get_right_from_block(parent) : get_left_from_block(parent);

        if (get_data_from_block(brother).color == block_color::RED)
        {
            if (from_left)
            {
                small_left_rotation(parent);
            } else
            {
                small_right_rotation(parent);
            }
            get_data_from_block(parent).color = block_color::RED;
            get_data_from_block(brother).color = block_color::BLACK;

            rebalance_black_leaf(parent, deleted);
        } else
        {
            void* far_nephew = from_left ? get_right_from_block(brother) : get_left_from_block(brother);
            void* near_nephew = from_left ? get_left_from_block(brother) : get_right_from_block(brother);

            if (far_nephew != nullptr && get_data_from_block(far_nephew).color == block_color::RED)
            {
                if (from_left)
                    small_left_rotation(parent);
                else
                    small_right_rotation(parent);

                get_data_from_block(brother).color = get_data_from_block(parent).color;
                get_data_from_block(parent).color = block_color::BLACK;
                get_data_from_block(far_nephew).color = block_color::BLACK;
            } else if (near_nephew != nullptr && get_data_from_block(near_nephew).color == block_color::RED)
            {
                if (from_left)
                {
                    big_left_rotation(parent);
                } else
                {
                    big_right_rotation(parent);
                }

                get_data_from_block(near_nephew).color = get_data_from_block(parent).color;
                get_data_from_block(parent).color = block_color::BLACK;
            } else
            {
                get_data_from_block(brother).color = block_color::RED;

                if (get_data_from_block(parent).color == block_color::RED)
                {
                    get_data_from_block(parent).color = block_color::BLACK;
                } else
                {
                    rebalance_black_leaf(get_parent_from_block(parent), parent);
                }
            }
        }
    }

//	check_ptr(parent);
//	check_ptr(deleted);
}

bool allocator_red_black_tree::is_left_subtree(void* child, void* parent) noexcept
{
    return child == get_left_from_block(parent);
}

void *allocator_red_black_tree::get_first(size_t size) const noexcept
{
    void* node = *get_first_block_ptr(_trusted_memory);

    while (node != nullptr)
    {
        if (get_block_size(node, _trusted_memory) >= size)
            return node;

        node = get_right_from_block(node);
    }

    return nullptr;
}

void *allocator_red_black_tree::get_best(size_t size) const noexcept
{
    void* node = *get_first_block_ptr(_trusted_memory);
    void* prev = nullptr;

    while (node != nullptr)
    {
        size_t tmp = get_block_size(node, _trusted_memory);
        if (tmp >= size)
        {
            prev = node;
        }

        if (tmp < size)
        {
            node = get_right_from_block(node);
        } else if (tmp > size)
        {
            node = get_left_from_block(node);
        } else
            node= nullptr;
    }

    return prev;
}

void *allocator_red_black_tree::get_worst(size_t size) const noexcept
{
    void* node = *get_first_block_ptr(_trusted_memory);
    void* prev = nullptr;

    while (node != nullptr)
    {
        if (get_block_size(node, _trusted_memory) >= size)
        {
            prev = node;
        }
        node = get_right_from_block(node);
    }

    return prev;
}

void allocator_red_black_tree::print_tree(void *block, size_t depth)
{
	if (block != nullptr)
	{
		print_tree(get_right_from_block(block), depth + 1);
		for (size_t i = 0; i < depth; ++i)
		{
			std::cout << '\t';
		}

		std::cout << get_block_size(block, _trusted_memory) << " " << (get_data_from_block(block).color == block_color::RED) << " " << get_data_from_block(block).occupied << std::endl;

		print_tree(get_left_from_block(block), depth + 1);
	}
}

bool allocator_red_black_tree::rb_iterator::operator==(const allocator_red_black_tree::rb_iterator &other) const noexcept
{
    return _block_ptr == other._block_ptr;
}

bool allocator_red_black_tree::rb_iterator::operator!=(const allocator_red_black_tree::rb_iterator &other) const noexcept
{
    return !(*this == other);
}

allocator_red_black_tree::rb_iterator &allocator_red_black_tree::rb_iterator::operator++() noexcept
{
    _block_ptr = get_forward_from_block(_block_ptr);
	return *this;
}

allocator_red_black_tree::rb_iterator allocator_red_black_tree::rb_iterator::operator++(int n)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_red_black_tree::rb_iterator::size() const noexcept
{
    return get_block_size(_block_ptr, _trusted);
}

void *allocator_red_black_tree::rb_iterator::operator*() const noexcept
{
    return _block_ptr;
}

allocator_red_black_tree::rb_iterator::rb_iterator()
{
    _block_ptr = nullptr;
    _trusted = nullptr;
}

allocator_red_black_tree::rb_iterator::rb_iterator(void *trusted)
{
    _block_ptr = reinterpret_cast<std::byte*>(trusted) + allocator_metadata_size;
    _trusted = trusted;
}

bool allocator_red_black_tree::rb_iterator::occupied() const noexcept
{
    return is_block_occupied(_block_ptr);
}
