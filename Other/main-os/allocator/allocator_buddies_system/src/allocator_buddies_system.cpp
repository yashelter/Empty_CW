#include <not_implemented.h>

#include "../include/allocator_buddies_system.h"

//using byte = unsigned char;


allocator_buddies_system::~allocator_buddies_system()
{
    debug_with_guard(get_typename() + " destructor called");
    get_mutex().~mutex();
    deallocate_with_guard(_trusted_memory);
}

allocator_buddies_system::allocator_buddies_system(
    allocator_buddies_system&& other) noexcept
{
    debug_with_guard("Move copy start");

    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;

    debug_with_guard("Move copy finish");
}

allocator_buddies_system& allocator_buddies_system::operator=(
    allocator_buddies_system&& other) noexcept
{
    debug_with_guard("Start move assign");
    if (this != &other)
    {
        std::swap(_trusted_memory, other._trusted_memory);
    }

    debug_with_guard("End move assign");
    return *this;
}

allocator_buddies_system::allocator_buddies_system(
    size_t space_size_power_of_two,
    allocator* parent_allocator,
    logger* logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (space_size_power_of_two < _minimal_k)
    {
        throw std::logic_error("Size smaller than the metadata of the one block");
    }

    size_t real_size = pow_2(space_size_power_of_two) + _allocator_meta;

    if (parent_allocator == nullptr)
    {
        try
        {
            _trusted_memory = ::operator new(real_size);
        } catch (std::bad_alloc& ex)
        {
            error_with_guard("Bad allocation memory");
            throw;
        }
    } else
    {
        try
        {
            _trusted_memory = parent_allocator->allocate(real_size, 1);
        }
        catch (std::bad_alloc& ex)
        {
            error_with_guard("Bad allocation memory from parent allocator");
            throw;
        }
    }
    fill_allocator_fields(space_size_power_of_two, parent_allocator, logger, allocate_fit_mode);

    debug_with_guard("Builded allocator " + get_typename());
}

inline void allocator_buddies_system::fill_allocator_fields(size_t space_size_power_of_two,
    allocator* parent_allocator,
    logger* logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    void* memory =_trusted_memory;

    *reinterpret_cast<class logger**>(memory) = logger;
    memory = slide(memory, sizeof(class logger*));

    *reinterpret_cast<allocator**>(memory) = parent_allocator;
    memory = slide(memory, sizeof(allocator*));

    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(memory) = allocate_fit_mode;
    memory = slide(memory, sizeof(allocator_with_fit_mode::fit_mode));

    *reinterpret_cast<byte*>(memory) = space_size_power_of_two;
    memory = slide(memory, sizeof(byte));

    auto mut = reinterpret_cast<std::mutex*>(memory);
    construct(mut);
    memory = slide(memory, sizeof(std::mutex));

    better_byte* first_block = reinterpret_cast<better_byte*>(memory);

    (*first_block).is_using = false;
    first_block->size = space_size_power_of_two - _minimal_k;
}

[[nodiscard]] void* allocator_buddies_system::allocate(
    size_t value_size,
    size_t values_count)
{

    std::lock_guard lock(get_mutex());

    trace_with_guard("Started allocation proccess");

    size_t free_space;
    size_t need_size = values_count * value_size + _block_meta;

    information_with_guard("Allocator before allocated: current condition of blocks: " + get_info_in_string(get_blocks_info(free_space)));
    information_with_guard("Free memory to allocate: " + std::to_string(free_space));
    trace_with_guard(get_typename() + " allocating " + std::to_string(need_size) + " bytes ");

    void* block_ptr;
    auto fit_mode = get_fit_mode();

    if (fit_mode == allocator_with_fit_mode::fit_mode::first_fit)
    {
        block_ptr = get_first_suitable(need_size);
    }
    else if (fit_mode == allocator_with_fit_mode::fit_mode::the_best_fit)
    {
        block_ptr = get_best_suitable(need_size);
    }
    else if (fit_mode == allocator_with_fit_mode::fit_mode::the_worst_fit)
    {
        block_ptr = get_worst_suitable(need_size);
    }
    else
    {
        throw std::logic_error("Not existing case");
    }

    if (block_ptr == nullptr)
    {
        error_with_guard("Bad allocation by " + get_typename() + " of " + std::to_string(need_size) + " bytes");
        throw std::bad_alloc();
    }

    while (get_size_block(block_ptr) >= (need_size << 1))
    {
        auto brother = reinterpret_cast<better_byte*>(block_ptr);
        --(brother->size);

        auto sister = reinterpret_cast<better_byte*>(get_twin(block_ptr));
        sister->is_using = false;
        sister->size = brother->size;
    }

    if (get_size_block(block_ptr) != need_size)
    {
        warning_with_guard(get_typename() + "changed allocating block size to " + 
                           std::to_string(get_size_block(block_ptr)));
    }

    auto first_twin = reinterpret_cast<better_byte*>(block_ptr);
    first_twin->is_using = true;

    *reinterpret_cast<void**>(first_twin + 1) = _trusted_memory;

  
    trace_with_guard(get_typename() + " allocated " + std::to_string(need_size) + " bytes ");

    information_with_guard("Condition of blocks after allocation: " + get_info_in_string(get_blocks_info(free_space)));
    information_with_guard("Available free space: " + std::to_string(free_space));

    return slide(block_ptr, _block_meta);
}

void allocator_buddies_system::deallocate(void* at)
{
    std::lock_guard lock(get_mutex());

    trace_with_guard("Started deallocate");

    size_t size_avaliable;
    information_with_guard("Blocks condition before deallocate: \n" + 
            get_info_in_string(get_blocks_info(size_avaliable)));

    information_with_guard("Available free space: " + std::to_string(size_avaliable));


    void* current_block = reinterpret_cast<std::byte*>(at) - _block_meta;

    if (*reinterpret_cast<void**>(reinterpret_cast<std::byte*>(at) - sizeof(void*)) != _trusted_memory)
    {
        error_with_guard("Tried to deallocate not allocator's property");
        throw std::logic_error("Tried to deallocate not allocator's property");
    }

    size_t current_block_size = get_size_block(current_block) - _block_meta;

    debug_with_guard("condition of block before deallocate: " + get_dump(reinterpret_cast<char*>(at), current_block_size));
    
    reinterpret_cast<better_byte*>(current_block)->is_using = false;

    void* twin = get_twin(current_block);

    // merge twins until meet using block
    while (get_size_block(current_block) < get_size_full() && 
           get_size_block(current_block) == get_size_block(twin) && !is_occured(twin))
    {
        void* left_twin = current_block < twin ? current_block : twin;

        auto current_meta = reinterpret_cast<better_byte*>(left_twin);
        ++current_meta->size;

        current_block = left_twin;
        twin = get_twin(current_block);
    }

    trace_with_guard("Ended deallocate");

    information_with_guard("Blocks condition after deallocate: \n" + get_info_in_string(get_blocks_info(size_avaliable)));

    information_with_guard("Available free space: " + std::to_string(size_avaliable));
}

inline void allocator_buddies_system::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    trace_with_guard("Setting new fitmode");
    std::lock_guard lock(get_mutex());

    void* ptr = slide(_trusted_memory, (sizeof(logger*) + sizeof(allocator*)));
    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr) = mode;
    trace_with_guard("Set new fitmode");
}

size_t allocator_buddies_system::pow_2(size_t power_of_2) noexcept
{
    constexpr const size_t number = 1;
    return number << power_of_2;
}

size_t allocator_buddies_system::next_pow_of_2(size_t number) noexcept
{
    size_t power = 1;
    while (power < number)
    {
        power <<= 1;
    }
    return power;
}

inline std::mutex& allocator_buddies_system::get_mutex() noexcept
{
    void* ptr = slide(_trusted_memory, _allocator_meta - sizeof(std::mutex));
    return *reinterpret_cast<std::mutex*>(ptr);
}

void* allocator_buddies_system::get_first_suitable(size_t need_size)  const noexcept
{
    for (auto it = begin_for_iter_twin(), end = end_for_iter_twin(); it != end; ++it)
    {
        if (!it.is_block_ocuppied() && it.size() >= need_size)
        {
            return it.get_ptr_iterator_twin();
        }
    }
    return nullptr;
}

void* allocator_buddies_system::get_worst_suitable(size_t need_size) const noexcept
{
    size_t max_size = 0;
    void* result = nullptr;

    for (auto it = begin_for_iter_twin(), end = end_for_iter_twin(); it != end; ++it)
    {
        if (!it.is_block_ocuppied() && it.size() >= need_size && it.size() > max_size)
        {
            max_size = it.size();
            result = it.get_ptr_iterator_twin();
        }
    }
    return result;
}

void* allocator_buddies_system::get_best_suitable(size_t need_size)  const noexcept
{
    size_t current_size = get_size_full();
    void* result = nullptr;

    for (auto it = begin_for_iter_twin(), end = end_for_iter_twin(); it != end; ++it)
    {
        if (!it.is_block_ocuppied() && it.size() >= need_size && it.size() < current_size)
        {
            current_size = it.size();
            result = it.get_ptr_iterator_twin();
        }
    }
    return result;

}

inline size_t allocator_buddies_system::get_size_full() const noexcept
{
    void* ptr = slide(_trusted_memory, (sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode)));
    return pow_2(*reinterpret_cast<unsigned char*>(ptr));
}

inline allocator_with_fit_mode::fit_mode& allocator_buddies_system::get_fit_mode() const noexcept
{
    void* ptr = slide(_trusted_memory, (sizeof(logger*) + sizeof(allocator*)));
    return *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(ptr);
}

inline size_t allocator_buddies_system::get_size_block(void* current_block) noexcept
{
    better_byte* byte_ptr = reinterpret_cast<better_byte*>(current_block);
    return pow_2((byte_ptr->size) + _minimal_k);
}

void* allocator_buddies_system::get_twin(void* current_block) noexcept
{
    size_t zero_point = reinterpret_cast<byte*>(current_block) - 
            (reinterpret_cast<byte*>(_trusted_memory) + _allocator_meta);
    size_t delta = zero_point ^ (get_size_block(current_block));
  
    return slide(_trusted_memory, _allocator_meta + delta);
}

bool allocator_buddies_system::is_occured(void* current_block) noexcept
{
    return reinterpret_cast<better_byte*>(current_block)->is_using;
}

allocator_buddies_system::iterator_allocator_buddies::iterator_allocator_buddies() : _ptr_block(nullptr) {}

allocator_buddies_system::iterator_allocator_buddies::iterator_allocator_buddies(void* ptr) : _ptr_block(ptr) {}

size_t allocator_buddies_system::iterator_allocator_buddies::size() const noexcept
{
    return get_size_block(_ptr_block);
}

void* allocator_buddies_system::iterator_allocator_buddies::get_ptr_iterator_twin() const noexcept
{
    return _ptr_block;
}

bool allocator_buddies_system::iterator_allocator_buddies::is_block_ocuppied() const noexcept
{
    return is_occured(_ptr_block);
}

allocator_buddies_system::iterator_allocator_buddies allocator_buddies_system::begin_for_iter_twin() const noexcept
{
    return { reinterpret_cast<std::byte*>(_trusted_memory) + _allocator_meta };
}

allocator_buddies_system::iterator_allocator_buddies allocator_buddies_system::end_for_iter_twin() const noexcept
{
    return { reinterpret_cast<std::byte*>(_trusted_memory) + _allocator_meta + get_size_full() };
}

allocator_buddies_system::iterator_allocator_buddies& allocator_buddies_system::iterator_allocator_buddies::operator++() noexcept
{
    _ptr_block = reinterpret_cast<std::byte*>(_ptr_block) + get_size_block(_ptr_block);
    return *this;
}

allocator_buddies_system::iterator_allocator_buddies allocator_buddies_system::iterator_allocator_buddies::operator++(int) noexcept
{
    auto temporary = *this;
    ++(*this);
    return temporary;
}

bool allocator_buddies_system::iterator_allocator_buddies::operator==(const allocator_buddies_system::iterator_allocator_buddies& oth) const noexcept
{
    return _ptr_block == oth._ptr_block;
}

bool allocator_buddies_system::iterator_allocator_buddies::operator!=(const allocator_buddies_system::iterator_allocator_buddies& oth) const noexcept
{
    return !(*this == oth);
}

std::string allocator_buddies_system::get_info_in_string(const std::vector<allocator_test_utils::block_info>& vec) noexcept
{
    std::ostringstream str;
    for (auto& it : vec)
    {
        if (it.is_block_occupied)
        {
            str << "<occup>";
        } else
            str << "<avail>";

        str << " <" + std::to_string(it.block_size) + "> | ";
    }
    return str.str();
}

std::string allocator_buddies_system::get_dump(char* at, size_t size)
{
    std::string result;
    for (size_t i = 0; i < size; ++i)
    {
        result += std::to_string(static_cast<int>(at[i])) + " ";
    }
    return result;
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info(size_t& full_size_avail) const noexcept
{
    full_size_avail = 0;
    std::vector<allocator_test_utils::block_info> result;

    for (auto it = begin_for_iter_twin(), end = end_for_iter_twin(); it != end; ++it)
    {
        if (!it.is_block_ocuppied())
        {
            full_size_avail += it.size();
        }
        result.push_back({ it.size(), it.is_block_ocuppied() });
    }

    return result;
}

inline logger* allocator_buddies_system::get_logger() const
{
    if (_trusted_memory == nullptr)
    {
        return nullptr;
    }
    return *reinterpret_cast<logger**>(_trusted_memory);
}

inline std::string allocator_buddies_system::get_typename() const noexcept
{
    return "allocator_buddies_system";
}

inline allocator* allocator_buddies_system::get_allocator() const
{
    auto byte_ptr = reinterpret_cast<std::byte*>(_trusted_memory);
    byte_ptr += sizeof(logger*);
    return *reinterpret_cast<allocator**>(byte_ptr);
}

inline void* allocator_buddies_system::slide(void* mem, size_t siz) const noexcept
{
    return reinterpret_cast<void*>(reinterpret_cast<byte*>(mem) + siz);
}