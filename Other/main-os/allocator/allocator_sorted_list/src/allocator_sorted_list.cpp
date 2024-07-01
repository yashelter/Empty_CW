#include <not_implemented.h>

#include "../include/allocator_sorted_list.h"

using byte = unsigned char;

void* allocator_sorted_list::slide(void* mem, size_t siz) noexcept
{
    return reinterpret_cast<void*>(reinterpret_cast<byte*>(mem) + siz);
}


allocator_sorted_list::free_blocks_iterator::free_blocks_iterator() : _ptr_free(nullptr) {}

allocator_sorted_list::free_blocks_iterator::free_blocks_iterator(void* ptr) : _ptr_free(*get_allocator_first_block(ptr)) {}

bool allocator_sorted_list::free_blocks_iterator::operator==(const allocator_sorted_list::free_blocks_iterator& oth) const noexcept
{
    return _ptr_free == oth._ptr_free;
}

bool allocator_sorted_list::free_blocks_iterator::operator!=(const allocator_sorted_list::free_blocks_iterator& oth) const noexcept
{
    return !(*this == oth);
}

allocator_sorted_list::free_blocks_iterator& allocator_sorted_list::free_blocks_iterator::operator++() noexcept
{
    _ptr_free = get_ptr_from_block(_ptr_free);
    return *this;
}

allocator_sorted_list::free_blocks_iterator allocator_sorted_list::free_blocks_iterator::operator++(int) noexcept
{
    auto temporary = *this;
    ++(*this);
    return temporary;
}

allocator_sorted_list::free_blocks_iterator allocator_sorted_list::get_begin() const noexcept
{
    return { _trusted_memory };
}

allocator_sorted_list::free_blocks_iterator allocator_sorted_list::get_end() const noexcept
{
    return {};
}

size_t allocator_sorted_list::free_blocks_iterator::size()
{
    return get_size_of_current_block(_ptr_free);
}

void* allocator_sorted_list::free_blocks_iterator::get_ptr_free_block() const noexcept
{
    return _ptr_free;
}

allocator_sorted_list::~allocator_sorted_list()
{
    debug_with_guard("allocator killing");
    get_mutex().~mutex();
    deallocate_with_guard(_trusted_memory);
}

allocator_sorted_list::allocator_sorted_list(
    allocator_sorted_list&& other) noexcept
{
    debug_with_guard("move copy start");
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
    debug_with_guard("move copy finish");
}

allocator_sorted_list& allocator_sorted_list::operator=(
    allocator_sorted_list&& other) noexcept
{
    debug_with_guard("assign move start");
    if (this == &other)
    {
        debug_with_guard("assign move finish");
        return *this;
    }
    std::swap(_trusted_memory, other._trusted_memory);
    debug_with_guard("assign move finish");
    return *this;
}

allocator_sorted_list::allocator_sorted_list(
    size_t space_size,
    allocator* parent_allocator,
    logger* logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    size_t real_size = space_size + _meta_allocator;

    if (parent_allocator == nullptr)
    {
        try
        {
            _trusted_memory = ::operator new(real_size);
        } catch (std::bad_alloc& ex)
        {
            if (logger != nullptr)
            {
                logger->error("Bad allocation memory");
            }
            throw;
        }
    } else
    {
        try
        {
            _trusted_memory = parent_allocator->allocate(real_size, 1);
        } catch (std::bad_alloc& ex)
        {
            if (logger != nullptr)
            {
                logger->error("Bad allocation memory from parent allocator");
            }
            throw;
        }
    }
    
    fill_allocator_fields(space_size, parent_allocator, logger, allocate_fit_mode);
    debug_with_guard("constructor finish");
}


inline void allocator_sorted_list::fill_allocator_fields(size_t siz,
    allocator* parent_allocator,
    logger* logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    void* memory = _trusted_memory;

    *reinterpret_cast<class logger**>(memory) = logger;
    memory = slide(memory, sizeof(class logger*));

    *reinterpret_cast<allocator**>(memory) = parent_allocator;
    memory = slide(memory, sizeof(allocator*));

    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(memory) = allocate_fit_mode;
    memory = slide(memory, sizeof(allocator_with_fit_mode::fit_mode));

    *reinterpret_cast<size_t*>(memory) = siz;
    memory = slide(memory, sizeof(size_t));

    auto mut = reinterpret_cast<std::mutex*>(memory);
    construct(mut);
    memory = slide(memory, sizeof(std::mutex));

    *reinterpret_cast<void**>(memory) = slide(memory,  sizeof(void*));
    memory = slide(memory, sizeof(void*));

    *reinterpret_cast<size_t*>(memory) = siz - _meta_block;
    memory = slide(memory, sizeof(size_t));

    *reinterpret_cast<void**>(memory) = nullptr;
}


[[nodiscard]] void* allocator_sorted_list::allocate(size_t value_size, size_t values_count)
{
    std::lock_guard lock(get_mutex());

    size_t real_size = value_size * values_count;
    size_t current_free_space = -1;

    trace_with_guard("Started allocation proccess");

    information_with_guard("Allocator before allocated: current condition of blocks: " + get_info_in_string(get_blocks_info(current_free_space)));
    information_with_guard("Free memory to allocate: " + std::to_string(current_free_space));
    trace_with_guard(get_typename() + " allocating " + std::to_string(real_size) + " bytes ");

    void* prev_block = nullptr;
    auto mode = get_fit_mode();
    
    if (mode == allocator_sorted_list::fit_mode::first_fit)
    {
        prev_block = get_first_suitable(real_size);
    }
    else if (mode == allocator_sorted_list::fit_mode::the_best_fit)
    {
        prev_block = get_best_suitable(real_size);
    }
    else if (mode == allocator_sorted_list::fit_mode::the_worst_fit)
    {
        prev_block = get_worst_suitable(real_size);
    }
    else
    {
        throw std::logic_error("Unknown fit mode");
    }

    if (prev_block == nullptr)
    {
        error_with_guard("Bad allocation by " + get_typename() + " of " + std::to_string(real_size) + " bytes");
        throw std::bad_alloc();
    }

    void* current_ptr = prev_block == _trusted_memory ? 
                *get_allocator_first_block(_trusted_memory) : get_ptr_from_block(prev_block);

    void* next_ptr = get_ptr_from_block(current_ptr);
    size_t size_founded_block = get_size_of_current_block(current_ptr);

    if (real_size + _meta_block > size_founded_block) // we can't add free block to list
    {
        warning_with_guard("Allocator with boundary tags changed allocating block size to " + std::to_string(size_founded_block));
        real_size = size_founded_block;
    }

    void* ptr = current_ptr;
    *reinterpret_cast<size_t*>(current_ptr) = real_size;
    ptr = slide(ptr, sizeof(size_t));

    *reinterpret_cast<void**>(ptr) = _trusted_memory;

    if (size_founded_block != real_size)// 0 or > meta
    {
        // spliting and add in list
        ptr = slide(current_ptr, _meta_block + get_size_of_current_block(current_ptr));
        *reinterpret_cast<size_t*>(ptr) = size_founded_block - real_size - _meta_block;
        ptr = slide(ptr, sizeof(size_t));

        *reinterpret_cast<void**>(ptr) = next_ptr;
        next_ptr = slide(current_ptr, _meta_block + get_size_of_current_block(current_ptr));
    }

    *get_ptr_from_previous_block(prev_block) = next_ptr;

    trace_with_guard(get_typename() + " allocated " + std::to_string(real_size) + " bytes ");

    information_with_guard("Condition of blocks after allocation: " + get_info_in_string(get_blocks_info(current_free_space)));
    information_with_guard("Available free space: " + std::to_string(current_free_space));

    return slide(current_ptr, _meta_block);
}

void allocator_sorted_list::deallocate(void* at)
{
    std::lock_guard lock(get_mutex());

    size_t size_availiable = -1;

    trace_with_guard("deallocate memory start");
    information_with_guard("Blocks condition before deallocate: \n" +
        get_info_in_string(get_blocks_info(size_availiable)));

    information_with_guard("Available free space: " + std::to_string(size_availiable));

    void* block_start = slide(at, -_meta_block);

    if (get_ptr_from_block(block_start) != _trusted_memory)
    {
        error_with_guard("Tried to deallocate not allocator's property");
        throw std::logic_error("Tried to deallocate not allocator's property");
    }

    debug_with_guard("Condition of block before deallocate: " + get_dump(reinterpret_cast<char*>(at), get_size_of_current_block(block_start)));

    void* prev_ptr = get_previous_block_for_occured(block_start);
    void* next_ptr;

    if (prev_ptr == _trusted_memory)
    {
        next_ptr = *get_allocator_first_block(_trusted_memory);
    }
    else
    {
        next_ptr = get_ptr_from_block(prev_ptr);
    }

    // merge with right bro
    if (next_ptr != nullptr && slide(block_start, _meta_block + get_size_of_current_block(block_start)) == next_ptr)
    {
        void *ptr = block_start;

        *reinterpret_cast<size_t*>(ptr) = get_size_of_current_block(block_start) + _meta_block + get_size_of_current_block(next_ptr);
        ptr = slide(ptr, sizeof(size_t));

        *reinterpret_cast<void**>(ptr) = get_ptr_from_block(next_ptr);
    }
    else
    {
        *get_ptr_from_previous_block(block_start) = next_ptr;
    }

    *get_ptr_from_previous_block(prev_ptr) = block_start; // set prev ptr to this

    // merge with left bro
    if (prev_ptr != _trusted_memory && slide(prev_ptr,  _meta_block + get_size_of_current_block(prev_ptr)) == block_start)
    {
        *reinterpret_cast<size_t*>(prev_ptr) = get_size_of_current_block(prev_ptr) + 
                                _meta_block + get_size_of_current_block(block_start);
        void *ptr = slide(prev_ptr, sizeof(size_t));
        *reinterpret_cast<void**>(ptr) = get_ptr_from_block(block_start);
        // ptr to left set up right and not need to change
    }

    trace_with_guard("Ended deallocate");
    information_with_guard("Blocks condition after deallocate: \n" + get_info_in_string(get_blocks_info(size_availiable)));
    information_with_guard("Available free space: " + std::to_string(size_availiable));
}

inline void allocator_sorted_list::set_fit_mode(allocator_with_fit_mode::fit_mode mode)
{
    std::lock_guard lock(get_mutex());
    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(
        slide(_trusted_memory, sizeof(logger*) + sizeof(allocator*))) = mode;
}

inline allocator* allocator_sorted_list::get_allocator() const
{
    return *reinterpret_cast<allocator**>(slide(_trusted_memory, sizeof(logger*)));
}

std::string allocator_sorted_list::get_info_in_string(const std::vector<allocator_test_utils::block_info>& vec) noexcept
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


std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info(size_t& space) const noexcept
{
    space = 0;
    std::vector<allocator_test_utils::block_info> result;

    auto ptr = get_begin();
    auto end = get_end();

    void* prev = _trusted_memory;

    if (ptr == end)
    {
        auto byte_ptr = slide(_trusted_memory, _meta_allocator);
        auto end_all = slide(_trusted_memory, _meta_allocator + get_allocator_full_space_size());

        while (byte_ptr != end_all)
        {
            result.push_back({ get_size_of_current_block(byte_ptr), true });
            byte_ptr = slide(byte_ptr, _meta_block + get_size_of_current_block(byte_ptr));
        }

    }

    for (; ptr != end; ++ptr)
    {
        size_t slide_d = prev == _trusted_memory ? _meta_allocator : _meta_block + get_size_of_current_block(prev);

        void* ptr_t = slide(prev, slide_d);

        while (ptr_t != ptr.get_ptr_free_block())
        {
            result.push_back({ get_size_of_current_block(ptr_t), true });
            ptr_t = slide(ptr_t, _meta_block + get_size_of_current_block(ptr_t));
        }

        space += ptr.size();
        result.push_back({ ptr.size(), false });

        auto check_end = *reinterpret_cast<void**>(slide(ptr.get_ptr_free_block(), sizeof(size_t)));
        if (check_end == nullptr)
        {
            ptr_t = slide(ptr.get_ptr_free_block(), _meta_block + ptr.size());
            auto a_end = slide(_trusted_memory,  _meta_allocator + get_allocator_full_space_size());

            while (ptr_t != a_end)
            {
                result.push_back({ get_size_of_current_block(ptr_t), true });
                ptr_t = slide(ptr_t, _meta_block + get_size_of_current_block(ptr_t));
            }
        }

        prev = ptr.get_ptr_free_block();
    }
    return result;
}

inline logger* allocator_sorted_list::get_logger() const
{
    return *reinterpret_cast<logger**>(_trusted_memory);
}

inline std::string allocator_sorted_list::get_typename() const noexcept
{
    return "allocator_sorted_list";
}

inline std::mutex& allocator_sorted_list::get_mutex() noexcept
{
    return *reinterpret_cast<std::mutex*>(slide(_trusted_memory, sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t)));
}

inline allocator_with_fit_mode::fit_mode& allocator_sorted_list::get_fit_mode() const noexcept
{
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(slide(_trusted_memory, sizeof(logger*) + sizeof(allocator*)));
}

void* allocator_sorted_list::get_first_suitable(size_t need_size)  const noexcept
{
    void* result = _trusted_memory;

    for (auto it = get_begin(), end = get_end(); it != end; ++it)
    {
        if (it.size() >= need_size)
        {
            return result;
        }
        result = it.get_ptr_free_block();
    }
    return nullptr;
}

void* allocator_sorted_list::get_worst_suitable(size_t need_size) const noexcept
{
    size_t max = 0;

    void* result = nullptr;
    void* prev = _trusted_memory;

    for (auto it = get_begin(), end = get_end(); it != end; ++it)
    {
        size_t current_size = it.size();
        if (current_size >= need_size && current_size > max)
        {
            max = current_size;
            result = prev;
        }
        prev = it.get_ptr_free_block();
    }

    return result;
}

void* allocator_sorted_list::get_best_suitable(size_t need_size)  const noexcept
{
    size_t max = get_allocator_full_space_size();

    void* result = nullptr;
    void* prev = _trusted_memory;

    for (auto it = get_begin(), end = get_end(); it != end; ++it)
    {
        size_t current_size = it.size();
        if (current_size >= need_size && current_size < max)
        {
            max = current_size;
            result = prev;
        }
        prev = it.get_ptr_free_block();
    }
    return result;
}

inline size_t allocator_sorted_list::get_allocator_full_space_size() const noexcept
{
    return *reinterpret_cast<size_t*>(slide(_trusted_memory, sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode)));
}

std::string allocator_sorted_list::get_dump(char* at, size_t size)
{
    std::string result;
    for (size_t i = 0; i < size; ++i)
    {
        result += std::to_string(static_cast<int>(at[i])) + " ";
    }
    return result;
}

inline void** allocator_sorted_list::get_allocator_first_block(void* ptr) noexcept
{
    return reinterpret_cast<void**>(slide(ptr, _meta_allocator - sizeof(void*)));
} 

inline void* allocator_sorted_list::get_ptr_from_block(void* current) noexcept
{
    return *reinterpret_cast<void**>(slide(current, sizeof(size_t)));
}

// return ptr of ptr to previous block
inline void** allocator_sorted_list::get_ptr_from_previous_block(void* ptr) const noexcept
{
    if (ptr == _trusted_memory)
    {
        return reinterpret_cast<void**>(slide(_trusted_memory, _meta_allocator - sizeof(void*)));
    }
    return reinterpret_cast<void**>(slide(ptr, sizeof(size_t)));
}

inline void* allocator_sorted_list::get_previous_block_for_occured(void* loaded_ptr) const noexcept
{
    void* prev = _trusted_memory;
    // обходим список, ищем наш блок
    for (auto it = get_begin(), end = get_end(); it != end; ++it)
    {
        auto diff = reinterpret_cast<byte*>(it.get_ptr_free_block()) - reinterpret_cast<byte*>(loaded_ptr);

        if (diff > 0)
        {
            return prev;
        }

        prev = it.get_ptr_free_block();
    }
    return prev;
}

inline size_t allocator_sorted_list::get_size_of_current_block(void* current_block) noexcept
{
    return *reinterpret_cast<size_t*>(current_block);
}

inline void** allocator_sorted_list::get_blocks_father(void* current_block) const noexcept
{
    return reinterpret_cast<void**>(slide(current_block, sizeof(size_t)));
}
