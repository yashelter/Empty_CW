#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BOUNDARY_TAGS_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BOUNDARY_TAGS_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <iterator>
#include <mutex>

class allocator_boundary_tags final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

    static constexpr const size_t allocator_metadata_size = sizeof(logger*) + sizeof(allocator*) + sizeof(allocator_with_fit_mode::fit_mode) +
            sizeof(size_t) + sizeof(std::mutex) + sizeof(void*);

    static constexpr const size_t occupied_block_metadata_size = sizeof(size_t) + sizeof(void*) + sizeof(void*) + sizeof(void*);

    static constexpr const size_t free_block_metadata_size = 0;

    void *_trusted_memory;

public:
    
    ~allocator_boundary_tags() override;
    
    allocator_boundary_tags(
        allocator_boundary_tags const &other) = delete;
    
    allocator_boundary_tags &operator=(
        allocator_boundary_tags const &other) = delete;
    
    allocator_boundary_tags(
        allocator_boundary_tags &&other) noexcept;
    
    allocator_boundary_tags &operator=(
        allocator_boundary_tags &&other) noexcept;

public:
    
    explicit allocator_boundary_tags(
        size_t space_size,
        allocator *parent_allocator = nullptr,
        logger *logger = nullptr,
        allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:
    
    [[nodiscard]] void *allocate(
        size_t value_size,
        size_t values_count) override;
    
    void deallocate(
        void *at) override;

public:
    
    inline void set_fit_mode(
        allocator_with_fit_mode::fit_mode mode) override;

public:
    
    std::vector<allocator_test_utils::block_info> get_blocks_info() const override;

private:

    std::vector<allocator_test_utils::block_info> get_blocks_info_inner() const override;

    inline allocator *get_allocator() const override;
    
    inline logger *get_logger() const override;
    
    inline std::string get_typename() const noexcept override;

    inline allocator_with_fit_mode::fit_mode& get_fit_mod() const noexcept;

    inline std::mutex& get_mutex() const noexcept;

    static inline size_t get_overall_size(void* trusted_memory) noexcept;

    void* get_first(size_t size) const noexcept;

    void* get_best(size_t size) const noexcept;

    void* get_worst(size_t size) const noexcept;

    static inline void** get_first_block_ptr(void* trusted_memory) noexcept;

    static inline size_t get_occupied_size(void* block_start) noexcept;

    static inline void* get_previous_from_occupied(void* block_start) noexcept;

    static inline void* get_next_from_occupied(void* block_start) noexcept;

    static inline void* get_parent_from_occupied(void* block_start) noexcept;

    static inline size_t get_next_free_size(void* occupied_block_start, void* trusted_memory) noexcept;

    size_t get_free_size() const noexcept;

    class boundary_iterator
    {
        void* _occupied_ptr;
        bool _occupied;
        void* _trusted_memory;

    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = void*;
        using reference = void*&;
        using pointer = void**;
        using difference_type = ptrdiff_t;

        bool operator==(const boundary_iterator&) const noexcept;

        bool operator!=(const boundary_iterator&) const noexcept;

        boundary_iterator& operator++() noexcept;

        boundary_iterator& operator--() noexcept;

        boundary_iterator operator++(int n);

        boundary_iterator operator--(int n);

        size_t size() const noexcept;

        bool occupied() const noexcept;

        void* operator*() const noexcept;

        void* get_ptr() const noexcept;

        boundary_iterator();

        boundary_iterator(void* trusted);
    };

    friend class boundary_iterator;

    boundary_iterator begin() const noexcept;

    boundary_iterator end() const noexcept;
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BOUNDARY_TAGS_H