#include <not_implemented.h>
#include <cstddef>
#include "../include/allocator_sorted_list.h"

allocator_sorted_list::~allocator_sorted_list()
{
    get_mutex().~mutex();
    deallocate_with_guard(_trusted_memory);
}

allocator_sorted_list::allocator_sorted_list(
    allocator_sorted_list &&other) noexcept
{
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
}

allocator_sorted_list &allocator_sorted_list::operator=(
    allocator_sorted_list &&other) noexcept
{
    std::swap(_trusted_memory, other._trusted_memory);
    return *this;
}

allocator_sorted_list::allocator_sorted_list(
    size_t space_size,
    allocator *parent_allocator,
    logger *logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (space_size < block_metadata_size)
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

    auto block_forward_ptr = reinterpret_cast<void**>(first_block_ptr + 1);

    *block_forward_ptr = nullptr;

    auto block_size_ptr = reinterpret_cast<size_t*>(block_forward_ptr + 1);

    *block_size_ptr = space_size - block_metadata_size;
}

[[nodiscard]] void *allocator_sorted_list::allocate(
    size_t value_size,
    size_t values_count)
{
    std::lock_guard lock(get_mutex());

    size_t real_size = value_size * values_count;

    debug_with_guard("Allocator sorted list started allocating " + std::to_string(real_size) + " bytes");

    void* prev_free;

    switch (get_fit_mod())
    {
        case allocator_with_fit_mode::fit_mode::first_fit:
            prev_free = get_first(real_size);
            break;
        case allocator_with_fit_mode::fit_mode::the_best_fit:
            prev_free = get_best(real_size);
            break;
        case allocator_with_fit_mode::fit_mode::the_worst_fit:
            prev_free = get_worst(real_size);
            break;
    }

    if (prev_free == nullptr)
    {
        error_with_guard("Allocator sorted list throwing bad_alloc while trying to allocate " + std::to_string(real_size) + " bytes");
        throw std::bad_alloc();
    }

    void* free_block_start = prev_free == _trusted_memory ? *get_first_block_ptr(_trusted_memory) : get_ptr_from_block(prev_free);

    size_t free_block_size = get_block_size(free_block_start);

    bool need_fraq = true;

    if (free_block_size < real_size + block_metadata_size)
    {
        warning_with_guard("Allocator sorted list changed allocating block size to " + std::to_string(free_block_size));
        real_size = free_block_size;
        need_fraq = false;
    }

    if (need_fraq)
    {
        void* new_free_block = reinterpret_cast<std::byte*>(free_block_start) + block_metadata_size + real_size;
        (prev_free == _trusted_memory ? *get_first_block_ptr(_trusted_memory) : *reinterpret_cast<void **>(prev_free)) = new_free_block;
        *reinterpret_cast<void **>(new_free_block) = get_ptr_from_block(free_block_start);
        auto size_ptr = reinterpret_cast<size_t*>(reinterpret_cast<std::byte*>(new_free_block) + sizeof(void*));
        *size_ptr = free_block_size - real_size - block_metadata_size;
        size_ptr = reinterpret_cast<size_t*>(reinterpret_cast<std::byte*>(free_block_start) + sizeof(void*));
        *size_ptr = real_size;
    } else
    {
        (prev_free == _trusted_memory ? *get_first_block_ptr(_trusted_memory) : *reinterpret_cast<void **>(prev_free)) = get_ptr_from_block(free_block_start);
    }

    *reinterpret_cast<void**>(free_block_start) = _trusted_memory;

    information_with_guard(std::to_string(get_free_size()));
    debug_with_guard(print_blocks());

    return reinterpret_cast<std::byte*>(free_block_start) + block_metadata_size;
}

void allocator_sorted_list::deallocate(
    void *at)
{
    std::lock_guard lock(get_mutex());

    void* block_start = reinterpret_cast<std::byte*>(at) - block_metadata_size;

    if (get_ptr_from_block(block_start) != _trusted_memory)
    {
        error_with_guard("Incorrect deallocation object");
        throw std::logic_error("Incorrect deallocation object");
    }

    size_t block_size = get_block_size(block_start);

    debug_with_guard(get_dump((char*)at, block_size));

    sorted_free_iterator prev;
    auto it = free_begin();

    for (auto sent = free_end(); it != sent && *it < block_start; ++it)
    {
        prev = it;
    }

    void* prev_free = *prev == nullptr ? _trusted_memory : *prev;

    void* next_free = *it;

    if (prev_free != _trusted_memory && reinterpret_cast<std::byte*>(prev_free) + get_block_size(prev_free) + block_metadata_size == block_start)
    {
        *reinterpret_cast<void**>(prev_free) = next_free;
        *reinterpret_cast<size_t*>(reinterpret_cast<std::byte*>(prev_free) + sizeof(void*)) += block_size + block_metadata_size;
        block_start = prev_free;
    } else if (prev_free != _trusted_memory)
    {
        *reinterpret_cast<void**>(prev_free) = block_start;
    } else
    {
        *get_first_block_ptr(_trusted_memory) = block_start;
    }

    if (reinterpret_cast<std::byte*>(block_start) + get_block_size(block_start) + block_metadata_size == next_free)
    {
        *reinterpret_cast<size_t*>(reinterpret_cast<std::byte*>(block_start) + sizeof(void*)) +=
                get_block_size(next_free) + block_metadata_size;
        *reinterpret_cast<void**>(block_start) = get_ptr_from_block(next_free);
    } else
    {
        *reinterpret_cast<void**>(block_start) = next_free;
    }

    information_with_guard(std::to_string(get_free_size()));
    debug_with_guard(print_blocks());
}

inline void allocator_sorted_list::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    std::lock_guard lock(get_mutex());
    get_fit_mod() = mode;
}

inline allocator *allocator_sorted_list::get_allocator() const
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<allocator**>(byte_ptr + sizeof(logger*));
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info() const noexcept
{
    std::lock_guard lock(get_mutex());

    return get_blocks_info_inner();
}

inline logger *allocator_sorted_list::get_logger() const
{
    return *reinterpret_cast<logger**>(_trusted_memory);
}

inline std::string allocator_sorted_list::get_typename() const
{
    return "allocator_sorted_list";
}

std::mutex &allocator_sorted_list::get_mutex() const noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);
    return *reinterpret_cast<std::mutex*>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t));
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info_inner() const
{
    std::vector<allocator_test_utils::block_info> res;

    std::back_insert_iterator<std::vector<allocator_test_utils::block_info>> inserter(res);

    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        inserter = {it.size(), it.occupied()};
    }

    return res;
}

allocator_with_fit_mode::fit_mode &allocator_sorted_list::get_fit_mod() const noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<fit_mode*>(byte_ptr + sizeof(logger*) + sizeof(allocator*));
}

size_t allocator_sorted_list::get_overall_size(void *trusted_memory) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(trusted_memory);

    return *reinterpret_cast<size_t*>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode));
}

void *allocator_sorted_list::get_first(size_t size) const noexcept
{
    void* res_ptr = _trusted_memory;
    for(auto it = free_begin(), sent = free_end(); it != sent; ++it)
    {
        if (it.size() >= size)
        {
            return res_ptr;
        }

        res_ptr = *it;
    }

    return nullptr;
}

void *allocator_sorted_list::get_best(size_t size) const noexcept
{
    sorted_free_iterator res;
    void* res_ptr = nullptr;
    void* prev_ptr = _trusted_memory;

    for(auto it = free_begin(), sent = free_end(); it != sent; ++it)
    {
        if (it.size() >= size && (it.size() < res.size() || *res == nullptr))
        {
            res = it;
            res_ptr = prev_ptr;
        }

        prev_ptr = *it;
    }

    return res_ptr;
}

void *allocator_sorted_list::get_worst(size_t size) const noexcept
{
    sorted_free_iterator res;
    void* res_ptr = nullptr;
    void* prev_ptr = _trusted_memory;

    for(auto it = free_begin(), sent = free_end(); it != sent; ++it)
    {
        if (it.size() >= size && (it.size() > res.size() || *res == nullptr))
        {
            res = it;
            res_ptr = prev_ptr;
        }

        prev_ptr = *it;
    }

    return res_ptr;
}

void *allocator_sorted_list::get_ptr_from_block(void *block_start) noexcept
{
    return *reinterpret_cast<void**>(block_start);
}

size_t allocator_sorted_list::get_block_size(void *block_start) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(block_start);

    return *reinterpret_cast<size_t*>(byte_ptr + sizeof(void*));
}

void **allocator_sorted_list::get_first_block_ptr(void *trusted_memory) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(trusted_memory);

    return reinterpret_cast<void**>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t) + sizeof(std::mutex));
}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::free_begin() const noexcept
{
    return {_trusted_memory};
}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::free_end() const noexcept
{
    return {};
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::begin() const noexcept
{
    return {_trusted_memory};
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::end() const noexcept
{
    return {};
}

size_t allocator_sorted_list::get_free_size() const noexcept
{
    size_t accum = 0;

    for (auto it = free_begin(), sent = free_end(); it != sent; ++it)
    {
        accum += it.size();
    }
    return accum;
}

bool allocator_sorted_list::sorted_free_iterator::operator==(
        const allocator_sorted_list::sorted_free_iterator & other) const noexcept
{
    return _free_ptr == other._free_ptr;
}

bool allocator_sorted_list::sorted_free_iterator::operator!=(
        const allocator_sorted_list::sorted_free_iterator &other) const noexcept
{
    return !(*this == other);
}

allocator_sorted_list::sorted_free_iterator &allocator_sorted_list::sorted_free_iterator::operator++() noexcept
{
    _free_ptr = get_ptr_from_block(_free_ptr);

    return *this;
}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::sorted_free_iterator::operator++(int n)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_sorted_list::sorted_free_iterator::size() const noexcept
{
    return _free_ptr == nullptr ? 0 : get_block_size(_free_ptr);
}

void *allocator_sorted_list::sorted_free_iterator::operator*() const noexcept
{
    return _free_ptr;
}

allocator_sorted_list::sorted_free_iterator::sorted_free_iterator() : _free_ptr(nullptr){}

allocator_sorted_list::sorted_free_iterator::sorted_free_iterator(void *trusted) : _free_ptr(*get_first_block_ptr(trusted)){}

bool allocator_sorted_list::sorted_iterator::operator==(const allocator_sorted_list::sorted_iterator & other) const noexcept
{
    return _current_ptr == other._current_ptr;
}

bool allocator_sorted_list::sorted_iterator::operator!=(const allocator_sorted_list::sorted_iterator &other) const noexcept
{
    return !(*this == other);
}

allocator_sorted_list::sorted_iterator &allocator_sorted_list::sorted_iterator::operator++() noexcept
{
    void* next_free;

    if (_free_ptr != _trusted_memory)
        next_free = get_ptr_from_block(_free_ptr);
    else
        next_free = *get_first_block_ptr(_trusted_memory);

    auto current_size = get_block_size(_current_ptr);

    void* next_block = reinterpret_cast<std::byte*>(_current_ptr) + current_size + block_metadata_size;

    if(next_free == nullptr && next_block >= reinterpret_cast<std::byte*>(_trusted_memory) + get_overall_size(_trusted_memory) + allocator_metadata_size)
    {
        _free_ptr = _current_ptr = nullptr;
    } else if (next_free == next_block)
    {
        _free_ptr = _current_ptr = next_block;
    } else
    {
        _current_ptr = next_block;
    }

    return *this;
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::sorted_iterator::operator++(int n)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_sorted_list::sorted_iterator::size() const noexcept
{
    if (_current_ptr == nullptr)
        return 0;
    return get_block_size(_current_ptr) + block_metadata_size;
}

void *allocator_sorted_list::sorted_iterator::operator*() const noexcept
{
    return _current_ptr;
}

allocator_sorted_list::sorted_iterator::sorted_iterator() : _current_ptr(nullptr), _trusted_memory(nullptr), _free_ptr(nullptr) {}

allocator_sorted_list::sorted_iterator::sorted_iterator(void *trusted) : _trusted_memory(trusted)
{
    void* start_of_memory = reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size;

    auto first_free = *get_first_block_ptr(_trusted_memory);

    if (start_of_memory == first_free)
    {
        _current_ptr = _free_ptr = first_free;
    } else
    {
        _current_ptr = start_of_memory;
        _free_ptr = _trusted_memory;
    }
}

bool allocator_sorted_list::sorted_iterator::occupied() const noexcept
{
    return _free_ptr != _current_ptr;
}
