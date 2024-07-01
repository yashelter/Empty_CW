#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <mutex>
#include <sstream>

using byte = unsigned char;

namespace __cnst
{
    constexpr size_t next_power_2(size_t number)
    {
        size_t power = 1;
        size_t k = 0;
        while (power < number)
        {
            power <<= 1;
            ++k;
        }
        return k;
    }
}


class allocator_buddies_system final :
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

    void* _trusted_memory = nullptr;

    struct better_byte
    {
        bool is_using : 1;
        unsigned char size : 7;
    };

    static constexpr const size_t _allocator_meta = sizeof(logger*) + sizeof(allocator*) +
                                    sizeof(allocator_with_fit_mode::fit_mode) + 
                                    sizeof(unsigned char) + sizeof(std::mutex);

    static constexpr const size_t _block_meta = sizeof(better_byte) + sizeof(void*);
    static constexpr const size_t _free_block_meta = sizeof(better_byte);

    static constexpr const size_t _minimal_k = __cnst::next_power_2(_block_meta);


public:

    ~allocator_buddies_system() override;

    allocator_buddies_system(
        allocator_buddies_system const& other) = delete;

    allocator_buddies_system& operator=(
        allocator_buddies_system const& other) = delete;

    allocator_buddies_system(
        allocator_buddies_system&& other) noexcept;

    allocator_buddies_system& operator=(
        allocator_buddies_system&& other) noexcept;

public:

    explicit allocator_buddies_system(
        size_t space_size_power_of_two,
        allocator* parent_allocator = nullptr,
        logger* logger = nullptr,
        allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:

    [[nodiscard]] void* allocate(
        size_t value_size,
        size_t values_count) override;

    void deallocate(void* at) override;

public:

    inline void set_fit_mode(
        allocator_with_fit_mode::fit_mode mode) override;

public:

    std::vector<allocator_test_utils::block_info> get_blocks_info(size_t& full_size_avail) const noexcept override;

private:

    inline logger* get_logger() const override;

    inline std::string get_typename() const noexcept override;

    inline allocator* get_allocator() const override;

private:
    inline void fill_allocator_fields(size_t space_size_power_of_two,
        allocator* parent_allocator,
        logger* logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode);

    inline void* slide(void* , size_t) const noexcept;

    static size_t pow_2(size_t power_of_2) noexcept;
    static size_t next_pow_of_2(size_t number) noexcept;

    static inline size_t get_size_block(void* current_block) noexcept;

    inline std::mutex& get_mutex() noexcept;

    void* get_twin(void* current_block) noexcept;

    void* get_first_suitable(size_t need_size)  const noexcept;
    void* get_worst_suitable(size_t need_size) const noexcept;
    void* get_best_suitable(size_t need_size)  const noexcept;

    inline size_t get_size_full() const noexcept;

    inline allocator_with_fit_mode::fit_mode& get_fit_mode() const noexcept;

    static bool is_occured(void* current_block) noexcept;

    static std::string get_info_in_string(
        const std::vector<allocator_test_utils::block_info>& vec) noexcept;
    static std::string get_dump(char* at, size_t size);

private:

    class iterator_allocator_buddies
    {
        void* _ptr_block;
 
    public:

        iterator_allocator_buddies();
        iterator_allocator_buddies(void* ptr);

        bool operator==(const iterator_allocator_buddies& oth) const noexcept;
        bool operator!=(const iterator_allocator_buddies& oth) const noexcept;

        iterator_allocator_buddies& operator++() noexcept;
        iterator_allocator_buddies operator++(int) noexcept;

        size_t size() const noexcept;

        void* get_ptr_iterator_twin() const noexcept;

        bool is_block_ocuppied() const noexcept;

    };

    friend class iterator_allocator_buddies;

    iterator_allocator_buddies begin_for_iter_twin() const noexcept;
    iterator_allocator_buddies end_for_iter_twin() const noexcept;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H
