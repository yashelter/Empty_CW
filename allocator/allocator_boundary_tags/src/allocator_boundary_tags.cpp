#include <not_implemented.h>
#include <cstddef>
#include "../include/allocator_boundary_tags.h"

allocator_boundary_tags::~allocator_boundary_tags()
{
    get_mutex().~mutex();
    deallocate_with_guard(_trusted_memory);
}

allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags &&other) noexcept
{
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags &&other) noexcept
{
    if (this != &other)
        std::swap(_trusted_memory, other._trusted_memory);
    return *this;
}

allocator_boundary_tags::allocator_boundary_tags(
    size_t space_size,
    allocator *parent_allocator,
    logger *logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (space_size < occupied_block_metadata_size)
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

    *first_block_ptr = nullptr;
}

[[nodiscard]] void *allocator_boundary_tags::allocate(
    size_t value_size,
    size_t values_count)
{
    std::lock_guard lock(get_mutex());

    size_t real_size = value_size * values_count + occupied_block_metadata_size;

    debug_with_guard("Allocator boundary tags started allocating " + std::to_string(real_size) + " bytes");

    void* prev_occupied;

    switch (get_fit_mod())
    {
        case allocator_with_fit_mode::fit_mode::first_fit:
            prev_occupied = get_first(real_size);
            break;
        case allocator_with_fit_mode::fit_mode::the_best_fit:
            prev_occupied = get_best(real_size);
            break;
        case allocator_with_fit_mode::fit_mode::the_worst_fit:
            prev_occupied = get_worst(real_size);
            break;
    }

    if (prev_occupied == nullptr)
    {
        error_with_guard("Allocator boundary tags throwing bad_alloc while trying to allocate " + std::to_string(real_size) + " bytes");
        throw std::bad_alloc();
    }

    size_t free_block_size = get_next_free_size(prev_occupied, _trusted_memory);

    if (free_block_size < real_size + occupied_block_metadata_size)
    {
        warning_with_guard("Allocator with boundary tags changed allocating block size to " + std::to_string(free_block_size));
        real_size = free_block_size;
    }

    void* free_block_start;

    if (prev_occupied == _trusted_memory)
    {
        free_block_start = reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size;
    } else
    {
        free_block_start = reinterpret_cast<std::byte*>(prev_occupied) + get_occupied_size(prev_occupied) + occupied_block_metadata_size;
    }

    auto size_ptr = reinterpret_cast<size_t*>(free_block_start);

    *size_ptr = real_size - occupied_block_metadata_size;

    auto back_ptr = reinterpret_cast<void**>(size_ptr + 1);

    *back_ptr = prev_occupied;

    auto forward_ptr = reinterpret_cast<void**>(back_ptr + 1);

    *forward_ptr = prev_occupied == _trusted_memory ? *get_first_block_ptr(_trusted_memory) : get_next_from_occupied(prev_occupied);

    auto parent_ptr = reinterpret_cast<void**>(forward_ptr + 1);

    *parent_ptr = _trusted_memory;

    void* next_block;

    if (prev_occupied == _trusted_memory)
    {
        next_block = *get_first_block_ptr(_trusted_memory);
    } else
    {
        next_block = get_next_from_occupied(prev_occupied);
    }

    if (next_block != nullptr)
    {
        auto byte_ptr = reinterpret_cast<std::byte*>(next_block);
        byte_ptr += sizeof(size_t);
        *reinterpret_cast<void**>(byte_ptr) = free_block_start;
    }

    if (prev_occupied == _trusted_memory)
    {
        *get_first_block_ptr(_trusted_memory) = free_block_start;
    } else
    {
        auto byte_ptr = reinterpret_cast<std::byte*>(prev_occupied);
        byte_ptr += sizeof(size_t) + sizeof(void*);
        *reinterpret_cast<void**>(byte_ptr) = free_block_start;
    }

    information_with_guard(std::to_string(get_free_size()));
    debug_with_guard(print_blocks());

    return reinterpret_cast<std::byte*>(free_block_start) + occupied_block_metadata_size;
}

void allocator_boundary_tags::deallocate(
    void *at)
{
    std::lock_guard lock(get_mutex());

    void* block_start = reinterpret_cast<std::byte*>(at) - occupied_block_metadata_size;

    if (get_parent_from_occupied(block_start) != _trusted_memory)
    {
        error_with_guard("Incorrect deallocation object");
        throw std::logic_error("Incorrect deallocation object");
    }

    size_t block_size = get_occupied_size(block_start);

    debug_with_guard(get_dump((char*)at, block_size));

    void* prev_block = get_previous_from_occupied(block_start);
    void* next_block = get_next_from_occupied(block_start);

    if (prev_block == _trusted_memory)
    {
        *get_first_block_ptr(_trusted_memory) = next_block;
    } else
    {
        auto byte_ptr = reinterpret_cast<std::byte*>(prev_block) + sizeof(size_t) + sizeof(void*);
        *reinterpret_cast<void**>(byte_ptr) = next_block;
    }

    if (next_block != nullptr)
    {
        auto byte_ptr = reinterpret_cast<std::byte*>(next_block) + sizeof(size_t);
        *reinterpret_cast<void**>(byte_ptr) = prev_block;
    }

    information_with_guard(std::to_string(get_free_size()));
    debug_with_guard(print_blocks());
}

inline void allocator_boundary_tags::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    std::lock_guard lock(get_mutex());
    get_fit_mod() = mode;
}

inline allocator *allocator_boundary_tags::get_allocator() const
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<allocator**>(byte_ptr + sizeof(logger*));
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const
{
    std::lock_guard lock(get_mutex());

    return get_blocks_info_inner();
}

inline logger *allocator_boundary_tags::get_logger() const
{
    return *reinterpret_cast<logger**>(_trusted_memory);
}

inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    return "allocator_boundary_tags";
}

allocator_with_fit_mode::fit_mode& allocator_boundary_tags::get_fit_mod() const noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<fit_mode*>(byte_ptr + sizeof(logger*) + sizeof(allocator*));
}

size_t allocator_boundary_tags::get_overall_size(void* trusted_memory) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(trusted_memory);

    return *reinterpret_cast<size_t*>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode));
}

std::mutex &allocator_boundary_tags::get_mutex() const noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<std::mutex*>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t));
}

void *allocator_boundary_tags::get_first(size_t size) const noexcept
{
    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied() && it.size() >= size)
        {
            return it.get_ptr();
        }
    }

    return nullptr;
}

void *allocator_boundary_tags::get_best(size_t size) const noexcept
{
    boundary_iterator res;

    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied() && it.size() >= size && (it.size() < res.size() || res.get_ptr() == nullptr))
        {
            res = it;
        }
    }

    return res.get_ptr();
}

void *allocator_boundary_tags::get_worst(size_t size) const noexcept
{
    boundary_iterator res;

    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied() && it.size() >= size && it.size() > res.size())
        {
            res = it;
        }
    }

    return res.get_ptr();
}

size_t allocator_boundary_tags::get_occupied_size(void *block_start) noexcept
{
    return *reinterpret_cast<size_t*>(block_start);
}

void *allocator_boundary_tags::get_previous_from_occupied(void *block_start) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(block_start);

    return *reinterpret_cast<void**>(byte_ptr + sizeof(size_t));
}

void *allocator_boundary_tags::get_next_from_occupied(void *block_start) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(block_start);

    return *reinterpret_cast<void**>(byte_ptr + sizeof(size_t) + sizeof(void*));
}

void *allocator_boundary_tags::get_parent_from_occupied(void *block_start) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(block_start);

    return *reinterpret_cast<void**>(byte_ptr + sizeof(size_t) + sizeof(void*) + sizeof(void*));
}

size_t allocator_boundary_tags::get_next_free_size(void *occupied_block_start, void* trusted_memory) noexcept
{
    if (occupied_block_start == trusted_memory)
    {
        return *get_first_block_ptr(trusted_memory) == nullptr ? get_overall_size(trusted_memory)
                            : reinterpret_cast<std::byte*>(*get_first_block_ptr(trusted_memory)) - reinterpret_cast<std::byte*>(trusted_memory) - allocator_metadata_size;
    } else
    {
        return get_next_from_occupied(occupied_block_start) == nullptr ? (reinterpret_cast<std::byte*>(trusted_memory) + allocator_metadata_size +
                get_overall_size(trusted_memory)) - (reinterpret_cast<std::byte*>(occupied_block_start) +
                get_occupied_size(occupied_block_start) + occupied_block_metadata_size) :
                reinterpret_cast<std::byte*>(get_next_from_occupied(occupied_block_start)) -
                (reinterpret_cast<std::byte*>(occupied_block_start) + get_occupied_size(occupied_block_start) + occupied_block_metadata_size);
    }
}

void **allocator_boundary_tags::get_first_block_ptr(void* trusted_memory) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(trusted_memory);

    return reinterpret_cast<void**>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t) + sizeof(std::mutex));
}

size_t allocator_boundary_tags::get_free_size() const noexcept
{
    size_t accum = 0;

    for (auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied())
        {
            accum += it.size();
        }
    }
    return accum;
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::begin() const noexcept
{
    return {_trusted_memory};
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::end() const noexcept
{
    return {};
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info_inner() const
{
    std::vector<allocator_test_utils::block_info> res;

    std::back_insert_iterator<std::vector<allocator_test_utils::block_info>> inserter(res);

    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        inserter = {it.size(), it.occupied()};
    }

    return res;
}

bool allocator_boundary_tags::boundary_iterator::operator==(
        const allocator_boundary_tags::boundary_iterator &other) const noexcept
{
    return _occupied_ptr == other._occupied_ptr && ((_occupied == other._occupied) || _occupied_ptr == nullptr || other._occupied_ptr ==
                                                                                                                          nullptr);
}

bool allocator_boundary_tags::boundary_iterator::operator!=(
        const allocator_boundary_tags::boundary_iterator & other) const noexcept
{
    return !(*this == other);
}

allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator++() noexcept
{
    if (!_occupied)
    {
        _occupied = true;
        _occupied_ptr = _trusted_memory == _occupied_ptr ? *get_first_block_ptr(_trusted_memory) : get_next_from_occupied(_occupied_ptr);
    } else
    {
        size_t current_size = get_occupied_size(_occupied_ptr);
        void* next_block = get_next_from_occupied(_occupied_ptr);

        if (next_block == reinterpret_cast<std::byte*>(_occupied_ptr) + current_size + occupied_block_metadata_size ||
                (next_block == nullptr && reinterpret_cast<std::byte*>(_occupied_ptr) + current_size + occupied_block_metadata_size ==
                                                  reinterpret_cast<std::byte*>(_trusted_memory) + get_overall_size(_trusted_memory) + allocator_metadata_size))
            _occupied_ptr = next_block;
        else
            _occupied = false;
    }


    return *this;
}

allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator--() noexcept
{
    if (!_occupied)
    {
        _occupied = true;
    } else
    {
        void* prev_block = get_previous_from_occupied(_occupied_ptr);

        if(prev_block != _trusted_memory)
        {
            auto size = get_occupied_size(prev_block);

            _occupied = reinterpret_cast<std::byte*>(prev_block) + size + occupied_block_metadata_size == _occupied_ptr;

            _occupied_ptr = prev_block;
        }
    }


    return *this;
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator++(int n)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator--(int n)
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

size_t allocator_boundary_tags::boundary_iterator::size() const noexcept
{
    if (_occupied_ptr == nullptr)
        return 0;
    return _occupied ? get_occupied_size(_occupied_ptr) + occupied_block_metadata_size : get_next_free_size(_occupied_ptr, _trusted_memory);
}

bool allocator_boundary_tags::boundary_iterator::occupied() const noexcept
{
    return _occupied;
}

void* allocator_boundary_tags::boundary_iterator::operator*() const noexcept
{
    return _occupied ? _occupied_ptr : reinterpret_cast<std::byte*>(_occupied_ptr) + get_occupied_size(_occupied_ptr) + occupied_block_metadata_size;
}

allocator_boundary_tags::boundary_iterator::boundary_iterator() : _trusted_memory(nullptr), _occupied_ptr(nullptr), _occupied(false){}

allocator_boundary_tags::boundary_iterator::boundary_iterator(void *trusted) : _trusted_memory(trusted), _occupied_ptr(trusted), _occupied(true)
{
    _occupied = *get_first_block_ptr(_trusted_memory) ==
                (reinterpret_cast<std::byte *>(_trusted_memory) + allocator_metadata_size);
    if (_occupied)
        _occupied_ptr = *get_first_block_ptr(_trusted_memory);
}

void *allocator_boundary_tags::boundary_iterator::get_ptr() const noexcept
{
    return _occupied_ptr;
}
