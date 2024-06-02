#include <not_implemented.h>
#include <cstddef>
#include "../include/allocator_buddies_system.h"

allocator_buddies_system::~allocator_buddies_system()
{
    get_mutex().~mutex();
    deallocate_with_guard(_trusted_memory);
}

allocator_buddies_system::allocator_buddies_system(
    allocator_buddies_system &&other) noexcept
{
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
}

allocator_buddies_system &allocator_buddies_system::operator=(
    allocator_buddies_system &&other) noexcept
{
    if (this != &other)
        std::swap(_trusted_memory, other._trusted_memory);
    return *this;
}

allocator_buddies_system::allocator_buddies_system(
    size_t space_size,
    allocator *parent_allocator,
    logger *logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (space_size < min_k)
    {
        throw std::logic_error("To small space");
    }

    size_t real_size = power_of_2(space_size) + allocator_metadata_size;

    _trusted_memory = (parent_allocator == nullptr) ? ::operator new(real_size) : parent_allocator->allocate(real_size, 1);

    auto logger_ptr = reinterpret_cast<class logger**>(_trusted_memory);

    *logger_ptr = logger;

    auto parent_allocator_ptr = reinterpret_cast<allocator**>(logger_ptr + 1);

    *parent_allocator_ptr = parent_allocator;

    auto fit_mode_ptr = reinterpret_cast<allocator_with_fit_mode::fit_mode*>(parent_allocator_ptr + 1);

    *fit_mode_ptr = allocate_fit_mode;

    auto size_ptr = reinterpret_cast<unsigned char*>(fit_mode_ptr + 1);

    *size_ptr = __detail::nearest_greater_k_of_2(power_of_2(space_size));

    auto mutex_ptr = reinterpret_cast<std::mutex*>(size_ptr + 1);

    construct(mutex_ptr);

    auto start = reinterpret_cast<block_metadata*>(reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size);

    start->occupied = false;
    start->size = __detail::nearest_greater_k_of_2(power_of_2(space_size)) - min_k;
}

[[nodiscard]] void *allocator_buddies_system::allocate(
    size_t value_size,
    size_t values_count)
{
    std::lock_guard lock(get_mutex());

    size_t real_size = value_size * values_count + occupied_block_metadata_size;

    debug_with_guard("Allocator buddies system started allocating " + std::to_string(real_size) + " bytes");

    void* free_block;

    switch (get_fit_mod())
    {
        case allocator_with_fit_mode::fit_mode::first_fit:
            free_block = get_first(real_size);
            break;
        case allocator_with_fit_mode::fit_mode::the_best_fit:
            free_block = get_best(real_size);
            break;
        case allocator_with_fit_mode::fit_mode::the_worst_fit:
            free_block = get_worst(real_size);
            break;
    }

    if (free_block == nullptr)
    {
        error_with_guard("Allocator boundary tags throwing bad_alloc while trying to allocate " + std::to_string(real_size) + " bytes");
        throw std::bad_alloc();
    }

    while (get_block_size(free_block) >= real_size * 2)
    {
        auto metadata = reinterpret_cast<block_metadata*>(free_block);
        --metadata->size;
        auto second_metadata = reinterpret_cast<block_metadata*>(get_buddy(free_block));
        second_metadata->occupied = false;
        second_metadata->size = metadata->size;
    }

    if (get_block_size(free_block) != real_size)
    {
        warning_with_guard("Allocator buddies system changed allocating block size to " + std::to_string(get_block_size(free_block)));
    }

    auto free_metadata = reinterpret_cast<block_metadata*>(free_block);
    free_metadata->occupied = true;

    auto parent_ptr = reinterpret_cast<void**>(free_metadata + 1);
    *parent_ptr = _trusted_memory;

    information_with_guard(std::to_string(get_free_size()));
    debug_with_guard(print_blocks());

    return reinterpret_cast<std::byte*>(free_block) + occupied_block_metadata_size;
}

void allocator_buddies_system::deallocate(void *at)
{
    std::lock_guard lock(get_mutex());

    void* block_start = reinterpret_cast<std::byte*>(at) - occupied_block_metadata_size;

    if (*reinterpret_cast<void**>(reinterpret_cast<std::byte*>(block_start) + sizeof(block_metadata)) != _trusted_memory)
    {
        error_with_guard("Incorrect deallocation object");
        throw std::logic_error("Incorrect deallocation object");
    }

    size_t block_size = get_block_size(block_start) - occupied_block_metadata_size;

    debug_with_guard(get_dump((char*)at, block_size));

    reinterpret_cast<block_metadata*>(block_start)->occupied = false;

    void* buddy = get_buddy(block_start);

    while(get_block_size(block_start) < get_overall_size(_trusted_memory) && get_block_size(block_start) == get_block_size(buddy) && !is_occupied(buddy))
    {
        void* interested_ptr = block_start < buddy ? block_start : buddy;

        auto metadata = reinterpret_cast<block_metadata*>(interested_ptr);
        ++metadata->size;

        block_start = interested_ptr;
        buddy = get_buddy(block_start);
    }

    information_with_guard(std::to_string(get_free_size()));
    debug_with_guard(print_blocks());
}

inline void allocator_buddies_system::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    std::lock_guard lock(get_mutex());
    get_fit_mod() = mode;
}

inline allocator *allocator_buddies_system::get_allocator() const
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<allocator**>(byte_ptr + sizeof(logger*));
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info() const noexcept
{
    std::lock_guard lock(get_mutex());

    return get_blocks_info_inner();
}

inline logger *allocator_buddies_system::get_logger() const
{
    return *reinterpret_cast<logger**>(_trusted_memory);
}

inline std::string allocator_buddies_system::get_typename() const
{
    return "allocator_buddies_system";
}

constexpr size_t allocator_buddies_system::nearest_greater_power_of_2(size_t size) noexcept
{
    int ones_counter = 0, index = -1;

    constexpr const size_t o = 1;

    for (int i = sizeof(size_t) * 8 - 1; i >= 0; --i)
    {
        if (size & (o << i))
        {
            if (ones_counter == 0)
                index = i;
            ++ones_counter;
        }
    }

    return ones_counter <= 1 ? (o << index) : (o << (index + 1));
}

size_t allocator_buddies_system::power_of_2(size_t size) noexcept
{
    constexpr const size_t o = 1;

    return o << size;
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info_inner() const
{
    std::vector<allocator_test_utils::block_info> res;

    std::back_insert_iterator<std::vector<allocator_test_utils::block_info>> inserter(res);

    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        inserter = {it.size(), it.occupied()};
    }

    return res;
}

allocator_with_fit_mode::fit_mode &allocator_buddies_system::get_fit_mod() const noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<fit_mode*>(byte_ptr + sizeof(logger*) + sizeof(allocator*));
}

std::mutex &allocator_buddies_system::get_mutex() const noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);

    return *reinterpret_cast<std::mutex*>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(unsigned char));
}

size_t allocator_buddies_system::get_overall_size(void *trusted_memory) noexcept
{
    auto byte_ptr = reinterpret_cast<std::byte*>(trusted_memory);

    return power_of_2(*reinterpret_cast<unsigned char*>(byte_ptr + sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode)));
}

void *allocator_buddies_system::get_first(size_t size) const noexcept
{
    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied() && it.size() >= size)
        {
            return *it;
        }
    }

    return nullptr;
}

void *allocator_buddies_system::get_best(size_t size) const noexcept
{
    buddy_iterator res;

    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied() && it.size() >= size && (it.size() < res.size() || *res == nullptr))
        {
            res = it;
        }
    }

    return *res;
}

void *allocator_buddies_system::get_worst(size_t size) const noexcept
{
    buddy_iterator res;

    for(auto it = begin(), sent = end(); it != sent; ++it)
    {
        if (!it.occupied() && it.size() >= size && (it.size() > res.size() || *res == nullptr))
        {
            res = it;
        }
    }

    return *res;
}

size_t allocator_buddies_system::get_block_size(void *block_start) noexcept
{
    return power_of_2(reinterpret_cast<block_metadata*>(block_start)->size + min_k);
}

size_t allocator_buddies_system::get_free_size() const noexcept
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

void *allocator_buddies_system::get_buddy(void *block) noexcept
{
    size_t offset = (reinterpret_cast<std::byte*>(block) - (reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size));

    offset ^= (static_cast<size_t>(1u) << (reinterpret_cast<block_metadata*>(block)->size + min_k));

    return reinterpret_cast<void*>(offset + reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size);
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::begin() const noexcept
{
    return {reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size};
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::end() const noexcept
{
    return {reinterpret_cast<std::byte*>(_trusted_memory) + allocator_metadata_size + get_overall_size(_trusted_memory)};
}

bool allocator_buddies_system::is_occupied(void *block) noexcept
{
    return reinterpret_cast<block_metadata*>(block)->occupied;
}

bool allocator_buddies_system::buddy_iterator::operator==(const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return _block == other._block;
}

bool allocator_buddies_system::buddy_iterator::operator!=(const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return !(*this == other);
}

allocator_buddies_system::buddy_iterator &allocator_buddies_system::buddy_iterator::operator++() noexcept
{
    _block = reinterpret_cast<std::byte*>(_block) + get_block_size(_block);

    return *this;
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::buddy_iterator::operator++(int n)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_buddies_system::buddy_iterator::size() const noexcept
{
    return get_block_size(_block);
}

bool allocator_buddies_system::buddy_iterator::occupied() const noexcept
{
    return is_occupied(_block);
}

void *allocator_buddies_system::buddy_iterator::operator*() const noexcept
{
    return _block;
}

allocator_buddies_system::buddy_iterator::buddy_iterator(void *start) : _block(start){}

allocator_buddies_system::buddy_iterator::buddy_iterator() : _block(nullptr){}
