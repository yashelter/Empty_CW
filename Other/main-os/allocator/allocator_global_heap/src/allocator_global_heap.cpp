#include <not_implemented.h>

#include "../include/allocator_global_heap.h"
#include <string>

allocator_global_heap::allocator_global_heap(
    logger* logger) : _logger(logger)
{
    debug_with_guard("[debug] Builded " + get_typename());
}

allocator_global_heap::~allocator_global_heap()
{
    debug_with_guard("[debug] Destroyed " +  get_typename());
}

allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept
{
    _logger = other._logger;
}

allocator_global_heap &allocator_global_heap::operator=(
    allocator_global_heap &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    debug_with_guard("Call move assign for" + get_typename());
    _logger = other._logger;
    return *this;
}

[[nodiscard]] void *allocator_global_heap::allocate(
    size_t value_size,
    size_t values_count)
{
    size_t need_memory = values_count * value_size + _size_meta;

    debug_with_guard("[debug] " + get_typename() + "started allocate " + std::to_string(need_memory) + " bytes");

    void* allocated_blocks;

    try
    {
        allocated_blocks = ::operator new(need_memory);
    } 
    catch (const std::bad_alloc& ex)
    {
        error_with_guard("Bad allocation mem");
        throw;
    }

    *reinterpret_cast<allocator_global_heap**>(static_cast<char*>(allocated_blocks)) = this;

    debug_with_guard("[debug] " + get_typename() + "finished allocate " + std::to_string(need_memory) + " bytes");

    return reinterpret_cast<unsigned char*>(allocated_blocks) + _size_meta;
}

void allocator_global_heap::deallocate(
    void *at)
{
    //debug_with_guard("Global heap allocator start to dealloc " + std::format("{}", at) + " check");

    allocator_global_heap* removable = *reinterpret_cast<allocator_global_heap**>(static_cast<unsigned char*>(at) - _size_meta);

    unsigned char* memory = reinterpret_cast<unsigned char*>(at) - _size_meta;
    if (removable != this)
    {
        //throw std::logic_error("Not owned block by allocator");
        error_with_guard("[error] Not owned block by allocator");
    }
    // 
    //debug_with_guard("global heap start deallocating " + std::to_string(*removable) + " bytes of memory with dump " +
       // get_dump(reinterpret_cast<char*>(at), *removable));

    ::operator delete(memory);

    debug_with_guard("deallocate finished");
}

inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const noexcept
{
    return "allocator_global_heap";
}