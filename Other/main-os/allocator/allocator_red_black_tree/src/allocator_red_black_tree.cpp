#include <not_implemented.h>

#include "../include/allocator_red_black_tree.h"

inline void* allocator_red_black_tree::slide(void* mem, size_t siz) noexcept
{
    return reinterpret_cast<void*>(reinterpret_cast<byte*>(mem) + siz);
}

allocator_red_black_tree::~allocator_red_black_tree()
{
    debug_with_guard("allocator killing");
    get_mutex().~mutex();
    deallocate_with_guard(_trusted_memory);
}


allocator_red_black_tree::allocator_red_black_tree(
    allocator_red_black_tree&& other) noexcept
{
    debug_with_guard("move copy start");
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
    debug_with_guard("move copy finish");
}

allocator_red_black_tree& allocator_red_black_tree::operator=(
    allocator_red_black_tree&& other) noexcept
{
    debug_with_guard("assign move start");
    if (this != &other)
    {
        std::swap(_trusted_memory, other._trusted_memory);
    }
    return *this;
}


inline void allocator_red_black_tree::fill_allocator_fields(size_t size,
    allocator* parent_allocator,
    logger* logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    void* ptr = _trusted_memory;

    *reinterpret_cast<class logger**>(ptr) = logger;
    ptr = slide(ptr, sizeof(class logger*));

    *reinterpret_cast<allocator**>(ptr) = parent_allocator;
    ptr = slide(ptr, sizeof(class allocator*));

    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr) = allocate_fit_mode;
    ptr = slide(ptr, sizeof(allocator_with_fit_mode::fit_mode));

    *reinterpret_cast<size_t*>(ptr) = size;
    ptr = slide(ptr, sizeof(size_t));

    auto mut = reinterpret_cast<std::mutex*>(ptr);
    construct(mut);
    ptr = slide(ptr, sizeof(std::mutex));

    auto first_forward = reinterpret_cast<void**>(ptr);

    *first_forward = reinterpret_cast<std::byte*>(_trusted_memory) + _meta_allocator;

    ptr = slide(ptr, sizeof(void*));
    auto first_free_block = reinterpret_cast<void*>(ptr);

    get_occup_color(first_free_block).occupied = false;
    get_occup_color(first_free_block)._color = color::Black;
    get_back(first_free_block) = nullptr;
    get_forward(first_free_block) = nullptr;
    get_parent(first_free_block) = nullptr;
    get_left(first_free_block) = nullptr;
    get_right(first_free_block) = nullptr;
}

allocator_red_black_tree::allocator_red_black_tree(
    size_t space_size,
    allocator* parent_allocator,
    logger* logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    size_t real_size_for_allocator = space_size + _meta_allocator;

    if (parent_allocator == nullptr)
    {
        try
        {
            _trusted_memory = ::operator new(real_size_for_allocator);
        } catch (std::bad_alloc& ex)
        {
            error_with_guard("Bad allocation memory");
            throw;
        }
    } else
    {
        try
        {
            _trusted_memory = parent_allocator->allocate(real_size_for_allocator, 1);

        } catch (...)
        {
            error_with_guard("Bad allocation memory from parent allocator");
            throw;
        }
    }

   
    fill_allocator_fields(space_size, parent_allocator, logger, allocate_fit_mode);
    debug_with_guard("constructor finish");

}

[[nodiscard]] void* allocator_red_black_tree::allocate(
    size_t value_size,
    size_t values_count)
{
    std::lock_guard lock(get_mutex());

    trace_with_guard("Started allocation proccess");

    size_t free_space = 0;
    auto fit_mode = get_fit_mode();
    size_t need_size = value_size * values_count;

    information_with_guard("Allocator before allocated: current condition of blocks: " + get_info_in_string(get_blocks_info(free_space)));
    information_with_guard("Free memory to allocate: " + std::to_string(free_space));
    trace_with_guard(get_typename() + " allocating " + std::to_string(need_size) + " bytes ");

    

    void* suitable_free = nullptr;

    if (fit_mode == allocator_with_fit_mode::fit_mode::first_fit)
    {
        suitable_free = get_first_suitable(need_size);
    } else if (fit_mode == allocator_with_fit_mode::fit_mode::the_best_fit)
    {
        suitable_free = get_best_suitable(need_size);
    } else if (fit_mode == allocator_with_fit_mode::fit_mode::the_worst_fit)
    {
        suitable_free = get_worst_suitable(need_size);
    } else
    {
        throw std::logic_error("Not existing case");
    }

    if (suitable_free == nullptr)
    {
        error_with_guard("bad allocation for " + std::to_string(need_size) + " bytes");

        throw std::bad_alloc();
    }

    remove_rb_tree(suitable_free);
    get_occup_color(suitable_free).occupied = true;
    get_parent(suitable_free) = _trusted_memory;

    size_t free_block_size = get_size_block(suitable_free, _trusted_memory);

    bool need_devide = true;
    if (free_block_size < need_size + _meta_free_block)
    {
        need_size = free_block_size;
        warning_with_guard("Allocator red black tree changed allocating block size to " + std::to_string(need_size));
        need_devide = false;
    }

    if (need_devide)
    {
        void* devided_block = reinterpret_cast<std::byte*>(suitable_free) + _meta_occup_block + need_size;

        get_forward(devided_block) = get_forward(suitable_free);
        get_back(devided_block) = suitable_free;
        get_forward(suitable_free) = devided_block;
        if (get_forward(devided_block) != nullptr)
        {
            get_back(get_forward(devided_block)) = devided_block;
        }
        get_occup_color(devided_block).occupied = false;
        get_parent(devided_block) = nullptr;

        insert_rb_tree(devided_block);
    }

    trace_with_guard(get_typename() + " allocated " + std::to_string(need_size) + " bytes ");

    information_with_guard("Condition of blocks after allocation: " + get_info_in_string(get_blocks_info(free_space)));
    information_with_guard("Available free space: " + std::to_string(free_space));

    //print_tree(*get_first_block(_trusted_memory));

    return reinterpret_cast<byte*>(suitable_free) + _meta_occup_block;

}

void allocator_red_black_tree::deallocate(void* at)
{
    std::lock_guard lock(get_mutex());

    trace_with_guard("Started deallocate");

    size_t size_avaliable;
    information_with_guard("Blocks condition before deallocate: \n" +
        get_info_in_string(get_blocks_info(size_avaliable)));

    information_with_guard("Available free space: " + std::to_string(size_avaliable));


    void* block_start = reinterpret_cast<byte*>(at) - _meta_occup_block;

    if (get_parent(block_start) != _trusted_memory)
    {
        error_with_guard("You wanna delete a block that does not belong to this allocator");
        throw std::logic_error("You wanna delete a block that does not belong to this allocator");
    }

    debug_with_guard("condition of block before deallocate: " + get_dump(reinterpret_cast<char*>(at), get_size_block(block_start, _trusted_memory)));

    size_t block_size = get_size_block(block_start, _trusted_memory);

    get_occup_color(block_start).occupied = false;

    if (get_back(block_start) != nullptr && !get_occup_color(get_back(block_start)).occupied)
    {
        void* tmp = block_start;
        block_start = get_back(block_start);

        remove_rb_tree(block_start);
        get_forward(block_start) = get_forward(tmp);

        if (get_forward(block_start) != nullptr)
        {
            get_back(get_forward(block_start)) = block_start;
        }
    }



    if (get_forward(block_start) != nullptr && get_occup_color(get_forward(block_start)).occupied == false)
    {
        void* tmp = get_forward(block_start);
        remove_rb_tree(tmp);

        get_forward(block_start) = get_forward(tmp);

        if (get_forward(block_start) != nullptr)
        {
            get_back(get_forward(block_start)) = block_start;
        }
    }

    insert_rb_tree(block_start);
    size_t space_size;

    trace_with_guard("deallocate from allocator finish");
    information_with_guard("allocator after deallocated: current condition of blocks: " + get_info_in_string(get_blocks_info(space_size)));
    information_with_guard("current available memory: " + std::to_string(get_all_free_size()));

    //print_tree(*get_first_block(_trusted_memory));

}

inline void allocator_red_black_tree::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    std::lock_guard lock(get_mutex());
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);
    byte_ptr += sizeof(logger*) + sizeof(allocator*);
    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(byte_ptr) = mode;
}

std::vector<allocator_test_utils::block_info> allocator_red_black_tree::get_blocks_info(size_t& full_size_avail) const noexcept
{
    //std::lock_guard lock(get_mutex());

    std::vector<allocator_test_utils::block_info> result;

    for (auto it = begin_iter(), end = end_iter(); it != end; ++it)
    {
        result.push_back({ it.size(), it.is_not_availiable() });
    }

    return result;

}

inline allocator* allocator_red_black_tree::get_allocator() const
{
    auto ptr = slide(_trusted_memory, sizeof(logger*));
    return *reinterpret_cast<allocator**>(ptr);
}

inline logger* allocator_red_black_tree::get_logger() const
{
    return *reinterpret_cast<logger**>(_trusted_memory);
}

inline std::string allocator_red_black_tree::get_typename() const noexcept
{
    return "allocator_red_black_tree";
}

inline std::mutex& allocator_red_black_tree::get_mutex() const noexcept
{
    auto ptr = slide(_trusted_memory, sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t));
    return *reinterpret_cast<std::mutex*>(ptr);
}

inline allocator_with_fit_mode::fit_mode& allocator_red_black_tree::get_fit_mode() const noexcept
{
    auto ptr = slide(_trusted_memory, sizeof(logger*) + sizeof(allocator*));
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr);
}

inline size_t allocator_red_black_tree::get_size_full(void* trusted) noexcept
{
    auto ptr = slide(trusted, sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode));
    return *reinterpret_cast<size_t*>(ptr);
}

inline size_t allocator_red_black_tree::get_size_block(void* current_block, void* trusted_mem) noexcept
{
    if (get_forward(current_block) == nullptr)
    {
        return reinterpret_cast<byte*>(trusted_mem) + _meta_allocator + get_size_full(trusted_mem) - reinterpret_cast<byte*>(current_block) - _meta_occup_block;
    }
    return reinterpret_cast<byte*>(get_forward(current_block)) - reinterpret_cast<byte*>(current_block) - _meta_occup_block;

}

void*& allocator_red_black_tree::get_back(void* current_block) noexcept
{
    auto ptr = slide(current_block, sizeof(_tree_color));
    return *reinterpret_cast<void**>(ptr);
}


void*& allocator_red_black_tree::get_forward(void* current_block) noexcept
{
    auto ptr = slide(current_block, sizeof(_tree_color) + sizeof(void*));
    return *reinterpret_cast<void**>(ptr);
}


allocator_red_black_tree::_tree_color& allocator_red_black_tree::get_occup_color(void* current_block) noexcept
{
    return *reinterpret_cast<_tree_color*>(current_block);
}

void*& allocator_red_black_tree::get_parent(void* current_block) noexcept
{
    auto ptr = slide(current_block, sizeof(_tree_color) + 2 * sizeof(void*));
    return *reinterpret_cast<void**>(ptr);
}

void*& allocator_red_black_tree::get_left(void* current_block) noexcept
{
    auto ptr = slide(current_block, sizeof(_tree_color) + 3 * sizeof(void*));
    return *reinterpret_cast<void**>(ptr);
}

void*& allocator_red_black_tree::get_right(void* current_block) noexcept
{
    auto ptr = slide(current_block, sizeof(_tree_color) + 4 * sizeof(void*));
    return *reinterpret_cast<void**>(ptr);
}

void** allocator_red_black_tree::get_first_block(void* trusted_mem) noexcept
{
    auto ptr = slide(trusted_mem, sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t) + sizeof(std::mutex));
    return reinterpret_cast<void**>(ptr);
}

bool allocator_red_black_tree::is_not_availiable(void* current_block) noexcept
{
    return get_occup_color(current_block).occupied;
}

void allocator_red_black_tree::update_parent_ptr(void* current_block, void* new_parent) noexcept
{
    if (get_parent(current_block) == nullptr)
    {
        *get_first_block(_trusted_memory) = new_parent;
    } else
    {
        if (is_left_subtree(current_block, get_parent(current_block)))
        {
            get_left(get_parent(current_block)) = new_parent;
        } else
        {
            get_right(get_parent(current_block)) = new_parent;
        }
    }
}

bool allocator_red_black_tree::is_left_subtree(void* child, void* parent) noexcept
{
    return child == get_left(parent);
}


void allocator_red_black_tree::small_right_rotate(void* joint) noexcept
{
    if (get_left(joint) != nullptr)
    {
        void* left_son = get_left(joint);

        update_parent_ptr(joint, left_son);
        get_parent(left_son) = get_parent(joint);

        void* right_son_of_left_son_of_joint = get_right(left_son);

        get_right(left_son) = joint;
        get_parent(joint) = left_son;
        get_left(joint) = right_son_of_left_son_of_joint;

        if (right_son_of_left_son_of_joint != nullptr)
        {
            get_parent(right_son_of_left_son_of_joint) = joint;
        }
    }
}

void allocator_red_black_tree::small_left_rotate(void* joint) noexcept
{
    if (get_right(joint) != nullptr)
    {
        void* right_son = get_right(joint);

        update_parent_ptr(joint, right_son);
        get_parent(right_son) = get_parent(joint);

        void* left_son_of_right_son_of_joint = get_left(right_son);

        get_left(right_son) = joint;
        get_parent(joint) = right_son;
        get_right(joint) = left_son_of_right_son_of_joint;

        if (left_son_of_right_son_of_joint != nullptr)
        {
            get_parent(left_son_of_right_son_of_joint) = joint;
        }
    }
}

void allocator_red_black_tree::big_right_rotate(void* joint) noexcept
{
    if (get_left(joint) != nullptr && get_right(get_left(joint)) != nullptr)
    {
        void* node = get_left(joint);

        small_left_rotate(node);
        small_right_rotate(joint);
    }
}

void allocator_red_black_tree::big_left_rotate(void* joint) noexcept
{
    if (get_right(joint) != nullptr && get_left(get_right(joint)) != nullptr)
    {
        void* node = get_right(joint);

        small_right_rotate(node);
        small_left_rotate(joint);
    }
}

void allocator_red_black_tree::print_tree(void* block, size_t depth)
{
    if (block != nullptr)
    {
        print_tree(get_right(block), depth + 1);
        for (size_t i = 0; i < depth; ++i)
        {
            std::cout << '\t';
        }

        std::string color = get_occup_color(block)._color == color::Red ? "Red" : "Black";

        std::cout << get_size_block(block, _trusted_memory) << " " << color << " " << get_occup_color(block).occupied << std::endl;

        print_tree(get_left(block), depth + 1);
    }
}


void allocator_red_black_tree::insert_rb_tree(void* current_block) noexcept
{
    void* top = *get_first_block(_trusted_memory);
    void* parent = nullptr;

    while (top != nullptr)
    {
        if (get_size_block(current_block, _trusted_memory) < get_size_block(top, _trusted_memory))
        {
            parent = top;
            top = get_left(top);
        } else
        {
            parent = top;
            top = get_right(top);
        }
    }

    get_parent(current_block) = parent;
    get_left(current_block) = nullptr;
    get_right(current_block) = nullptr;
    //    get_forward(current_block) = nullptr;
    //    get_back(current_block) = nullptr;
    get_occup_color(current_block).occupied = false;
    get_occup_color(current_block)._color = color::Red;

    if (parent == nullptr)
    {
        *get_first_block(_trusted_memory) = current_block;
    } else
    {
        if (get_size_block(current_block, _trusted_memory) >= get_size_block(parent, _trusted_memory))
        {
            get_right(parent) = current_block;
        } else
        {
            get_left(parent) = current_block;
        }
    }

    bool need_to_continue = true;
    if (parent == nullptr)
    {
        get_occup_color(current_block)._color = color::Black;
        return;
    }
    bool up = is_left_subtree(current_block, parent);

    while (need_to_continue)
    {
        if (get_occup_color(parent)._color == color::Black)
        {
            break;
        }
        if (get_occup_color(parent)._color == color::Red)
        {
            void* grand = get_parent(parent);

            if (get_right(grand) == parent)
            {
                if ((get_left(grand) == nullptr || get_occup_color(get_left(grand))._color == color::Black) && !up)
                {
                    get_occup_color(grand)._color = color::Red;
                    small_left_rotate(grand);
                    get_occup_color(get_parent(grand))._color = color::Black;
                    break;
                } else if ((get_left(grand) == nullptr || get_occup_color(get_left(grand))._color == color::Black) && up)
                {
                    get_occup_color(grand)._color = color::Red;
                    big_left_rotate(grand);
                    get_occup_color(get_parent(grand))._color = color::Black;
                    break;
                } else if (get_occup_color(get_left(grand))._color == color::Red)
                {
                    get_occup_color(grand)._color = color::Red;
                    get_occup_color(get_left(grand))._color = color::Black;
                    get_occup_color(get_right(grand))._color = color::Black;
                    current_block = grand;
                }
            } else if (get_left(grand) == parent)
            {
                if ((get_right(grand) == nullptr || get_occup_color(get_right(grand))._color == color::Black) && up)
                {
                    get_occup_color(grand)._color = color::Red;
                    small_right_rotate(grand);
                    get_occup_color(get_parent(grand))._color = color::Black;
                    break;
                } else if ((get_right(grand) == nullptr || get_occup_color(get_right(grand))._color == color::Black) && !up)
                {
                    get_occup_color(grand)._color = color::Red;
                    big_right_rotate(grand);
                    get_occup_color(get_parent(grand))._color = color::Black;
                    break;
                } else if (get_occup_color(get_right(grand))._color == color::Red)
                {
                    get_occup_color(grand)._color = color::Red;
                    get_occup_color(get_left(grand))._color = color::Black;
                    get_occup_color(get_right(grand))._color = color::Black;
                    current_block = grand;
                }
            }
        }

        if (get_parent(current_block) == nullptr)
        {
            get_occup_color(current_block)._color = color::Black;
            break;
        }

        parent = get_parent(current_block);
        up = is_left_subtree(current_block, parent);
    }
}


void allocator_red_black_tree::remove_rb_tree(void* current_block) noexcept
{
    void* parent;
    bool need_check_upper = false;

    if (get_right(current_block) == nullptr && get_left(current_block) == nullptr)
    {
        parent = get_parent(current_block);
        update_parent_ptr(current_block, nullptr);
        need_check_upper = get_occup_color(current_block)._color == color::Black;
    } else if (get_right(current_block) == nullptr || get_left(current_block) == nullptr)
    {
        void* button_node = get_right(current_block) != nullptr ? get_right(current_block) : get_left(current_block);

        get_occup_color(button_node)._color = color::Black;

        update_parent_ptr(current_block, button_node);
        get_parent(button_node) = get_parent(current_block);

    } else
    {
        void* button_node = get_left(current_block);

        while (get_right(button_node) != nullptr)
        {
            button_node = get_right(button_node);
        }

        need_check_upper = get_left(button_node) == nullptr && get_occup_color(button_node)._color == color::Black;

        parent = get_parent(button_node);

        if (get_occup_color(button_node)._color == color::Black && get_left(button_node) != nullptr)
        {
            get_occup_color(get_left(button_node))._color = color::Black;
        }

        update_parent_ptr(current_block, button_node);
        get_right(button_node) = get_right(current_block);
        get_parent(get_right(button_node)) = button_node;

        if (get_parent(button_node) != current_block)
        {
            update_parent_ptr(button_node, get_left(button_node));
            if (get_left(button_node) != nullptr)
            {
                get_parent(get_left(button_node)) = get_parent(button_node);
            }
            get_left(button_node) = get_left(current_block);
            get_parent(get_left(button_node)) = button_node;
        } else
        {
            parent = button_node;
        }

        get_occup_color(button_node)._color = get_occup_color(current_block)._color;
        get_parent(button_node) = get_parent(current_block);

    }

    if (need_check_upper)
    {
        rebalance(parent);
    }
}

void allocator_red_black_tree::rebalance(void* parent, void* deleted)
{
    if (parent == nullptr)
    {
        if (deleted != nullptr)
        {
            get_occup_color(deleted)._color = color::Black;
        }
    } else
    {
        bool up = is_left_subtree(deleted, parent);

        void* bro = up ? get_right(parent) : get_left(parent);

        if (get_occup_color(bro)._color == color::Red)
        {
            up ? small_left_rotate(parent) : small_right_rotate(parent);

            get_occup_color(parent)._color = color::Red;
            get_occup_color(bro)._color = color::Black;

            rebalance(parent, deleted);
        } else
        {
            void* far_cousin = up ? get_right(bro) : get_left(bro);
            void* near_cousin = up ? get_left(bro) : get_right(bro);

            if (far_cousin != nullptr && get_occup_color(far_cousin)._color == color::Red)
            {
                up ? small_left_rotate(parent) : small_right_rotate(parent);

                get_occup_color(bro)._color = get_occup_color(parent)._color;
                get_occup_color(parent)._color = color::Black;
                get_occup_color(far_cousin)._color = color::Black;
            } else if (near_cousin != nullptr && get_occup_color(near_cousin)._color == color::Red)
            {
                up ? big_left_rotate(parent) : big_right_rotate(parent);

                get_occup_color(near_cousin)._color = get_occup_color(parent)._color;
                get_occup_color(parent)._color = color::Black;
            } else
            {
                get_occup_color(bro)._color = color::Red;

                if (get_occup_color(parent)._color == color::Red)
                {
                    get_occup_color(parent)._color = color::Black;
                } else
                {
                    rebalance(get_parent(parent), parent);
                }
            }
        }
    }
}

void* allocator_red_black_tree::get_first_suitable(size_t size) const noexcept
{
    void* result = *get_first_block(_trusted_memory);

    while (result != nullptr)
    {
        if (get_size_block(result, _trusted_memory) >= size)
        {
            return result;
        }
        result = get_right(result);
    }
    return result;
}

void* allocator_red_black_tree::get_worst_suitable(size_t size) const noexcept
{
    void* result = nullptr;
    void* node = *get_first_block(_trusted_memory);

    while (node != nullptr)
    {
        if (get_size_block(node, _trusted_memory) >= size)
        {
            result = node;
        }
        node = get_right(node);
    }

    return result;
}

void* allocator_red_black_tree::get_best_suitable(size_t size) const noexcept
{
    void* result = nullptr;
    void* node = *get_first_block(_trusted_memory);

    while (node != nullptr)
    {
        size_t size_of_node = get_size_block(node, _trusted_memory);
        if (size_of_node >= size)
        {
            result = node;
        }

        if (size_of_node > size)
        {
            node = get_left(node);
        } else if (size_of_node < size)
        {
            node = get_right(node);
        } else
        {
            node = nullptr;
        }
    }

    return result;
}

allocator_red_black_tree::iterator::iterator() : _ptr(nullptr), _crutch(nullptr) {}

allocator_red_black_tree::iterator::iterator(void* trust)
{
    _ptr = reinterpret_cast<void*>(reinterpret_cast<std::byte*>(trust) + _meta_allocator);
    _crutch = trust;
}

allocator_red_black_tree::iterator allocator_red_black_tree::begin_iter() const noexcept
{
    return { _trusted_memory };
}

allocator_red_black_tree::iterator allocator_red_black_tree::end_iter() const noexcept
{
    return {};
}

bool allocator_red_black_tree::iterator::operator==(const allocator_red_black_tree::iterator& oth) const noexcept
{
    return _ptr == oth._ptr;
}

bool allocator_red_black_tree::iterator::operator!=(const allocator_red_black_tree::iterator& oth) const noexcept
{
    return !(*this == oth);
}

allocator_red_black_tree::iterator& allocator_red_black_tree::iterator::operator++() noexcept
{
    _ptr = get_forward(_ptr);
    return *this;
}

allocator_red_black_tree::iterator allocator_red_black_tree::iterator::operator++(int) noexcept
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_red_black_tree::iterator::size()
{
    return get_size_block(_ptr, _crutch);
}

void* allocator_red_black_tree::iterator::get_ptr_free_block() const noexcept
{
    return _ptr;
}

bool allocator_red_black_tree::iterator::is_not_availiable() const noexcept
{
    return get_occup_color(_ptr).occupied;
}

std::string allocator_red_black_tree::get_dump(char* at, size_t size)
{
    std::string result;
    for (size_t i = 0; i < size; ++i)
    {
        result += std::to_string(static_cast<int>(at[i])) + " ";
    }
    return result;
}

std::string allocator_red_black_tree::get_info_in_string(const std::vector<allocator_test_utils::block_info>& vec) noexcept
{
    std::ostringstream str;
    for (auto& it : vec)
    {
        if (it.is_block_occupied)
        {
            str << "<occup>";
        } else
            str << "<avail>";

        str << "<" + std::to_string(it.block_size) + "> | ";
    }
    return str.str();
}

size_t allocator_red_black_tree::get_all_free_size() const noexcept
{
    size_t res = 0;

    for (auto it = begin_iter(), end = end_iter(); it != end; ++it)
    {
        if (!it.is_not_availiable())
            res += it.size();
    }
    return res;
}




