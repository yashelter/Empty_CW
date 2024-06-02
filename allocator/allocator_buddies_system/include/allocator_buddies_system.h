#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <mutex>
#include <cmath>

template <int a>
struct fact
{
    static const int val = fact<a-1>::val * a;
};

template<>
struct fact<0>
{
    static const int val = 1;
};

template<int a>
int fact_v = fact<a>::val;

namespace __detail
{
    constexpr size_t nearest_greater_k_of_2(size_t size) noexcept
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

        return ones_counter <= 1 ? index : index + 1;
    }
}

class allocator_buddies_system final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

    static size_t power_of_2(size_t size) noexcept;

    static constexpr size_t nearest_greater_power_of_2(size_t size) noexcept;

    struct block_metadata
    {
        bool occupied : 1;
        unsigned char size : 7;
    };

    void *_trusted_memory;

    static constexpr const size_t allocator_metadata_size = sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(unsigned char) + sizeof(std::mutex);

    static constexpr const size_t occupied_block_metadata_size = sizeof(block_metadata) + sizeof(void*);

    static constexpr const size_t free_block_metadata_size = sizeof(block_metadata);

    static constexpr const size_t min_k = __detail::nearest_greater_k_of_2(occupied_block_metadata_size);

public:

    explicit allocator_buddies_system(
            size_t space_size_power_of_two,
            allocator *parent_allocator = nullptr,
            logger *logger = nullptr,
            allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

    allocator_buddies_system(
        allocator_buddies_system const &other) =delete;
    
    allocator_buddies_system &operator=(
        allocator_buddies_system const &other) =delete;
    
    allocator_buddies_system(
        allocator_buddies_system &&other) noexcept;
    
    allocator_buddies_system &operator=(
        allocator_buddies_system &&other) noexcept;

    ~allocator_buddies_system() override;

public:
    
    [[nodiscard]] void *allocate(
        size_t value_size,
        size_t values_count) override;
    
    void deallocate(
        void *at) override;

    inline void set_fit_mode(
        allocator_with_fit_mode::fit_mode mode) override;


    std::vector<allocator_test_utils::block_info> get_blocks_info() const noexcept override;

private:
    
    inline allocator *get_allocator() const override;
    
    inline logger *get_logger() const override;
    
    inline std::string get_typename() const override;

    std::vector<allocator_test_utils::block_info> get_blocks_info_inner() const override;

    inline allocator_with_fit_mode::fit_mode& get_fit_mod() const noexcept;

    inline std::mutex& get_mutex() const noexcept;

    static inline size_t get_overall_size(void* trusted_memory) noexcept;

    void* get_first(size_t size) const noexcept;
    
    void* get_best(size_t size) const noexcept;

    void* get_worst(size_t size) const noexcept;

    static inline size_t get_block_size(void* block_start) noexcept;

    size_t get_free_size() const noexcept;

    void* get_buddy(void* block) noexcept;

    static bool is_occupied(void* block) noexcept;

    class buddy_iterator
    {
        void* _block;

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = void*;
        using reference = void*&;
        using pointer = void**;
        using difference_type = ptrdiff_t;

        bool operator==(const buddy_iterator&) const noexcept;

        bool operator!=(const buddy_iterator&) const noexcept;

        buddy_iterator& operator++() noexcept;

        buddy_iterator operator++(int n);

        size_t size() const noexcept;

        bool occupied() const noexcept;

        void* operator*() const noexcept;

        buddy_iterator();

        buddy_iterator(void* start);
    };

    friend class buddy_iterator;

    buddy_iterator begin() const noexcept;

    buddy_iterator end() const noexcept;
    
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BUDDIES_SYSTEM_H
