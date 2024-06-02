#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_SORTED_LIST_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_SORTED_LIST_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <iterator>
#include <mutex>

class allocator_sorted_list final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:
    
    void *_trusted_memory;

    static constexpr const size_t allocator_metadata_size = sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t) + sizeof(std::mutex) + sizeof(void*);

    static constexpr const size_t block_metadata_size = sizeof(void*) + sizeof(size_t);

public:

    explicit allocator_sorted_list(
            size_t space_size,
            allocator *parent_allocator = nullptr,
            logger *logger = nullptr,
            allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);
    
    allocator_sorted_list(
        allocator_sorted_list const &other) =delete;
    
    allocator_sorted_list &operator=(
        allocator_sorted_list const &other) =delete;
    
    allocator_sorted_list(
        allocator_sorted_list &&other) noexcept;
    
    allocator_sorted_list &operator=(
        allocator_sorted_list &&other) noexcept;

    ~allocator_sorted_list() override;
    
    [[nodiscard]] void *allocate(
        size_t value_size,
        size_t values_count) override;
    
    void deallocate(
        void *at) override;
    
    inline void set_fit_mode(
        allocator_with_fit_mode::fit_mode mode) override;

    std::vector<allocator_test_utils::block_info> get_blocks_info() const noexcept override;

private:

    std::vector<allocator_test_utils::block_info> get_blocks_info_inner() const override;

    inline allocator *get_allocator() const override;
    
    inline logger *get_logger() const override;
    
    inline std::string get_typename() const override;

    inline std::mutex& get_mutex() const noexcept;

    inline allocator_with_fit_mode::fit_mode& get_fit_mod() const noexcept;

    static inline size_t get_overall_size(void* trusted_memory) noexcept;

    static inline void** get_first_block_ptr(void* trusted_memory) noexcept;

    void* get_first(size_t size) const noexcept;

    void* get_best(size_t size) const noexcept;

    void* get_worst(size_t size) const noexcept;

    static void* get_ptr_from_block(void* block_start) noexcept;

    static size_t get_block_size(void* block_start) noexcept;

    size_t get_free_size() const noexcept;

    class sorted_free_iterator
    {
        void* _free_ptr;

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = void*;
        using reference = void*&;
        using pointer = void**;
        using difference_type = ptrdiff_t;

        bool operator==(const sorted_free_iterator&) const noexcept;

        bool operator!=(const sorted_free_iterator&) const noexcept;

        sorted_free_iterator& operator++() noexcept;

        sorted_free_iterator operator++(int n);

        size_t size() const noexcept;

        void* operator*() const noexcept;

        sorted_free_iterator();

        sorted_free_iterator(void* trusted);
    };

    class sorted_iterator
    {
        void* _free_ptr;
        void* _current_ptr;
        void* _trusted_memory;

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = void*;
        using reference = void*&;
        using pointer = void**;
        using difference_type = ptrdiff_t;

        bool operator==(const sorted_iterator&) const noexcept;

        bool operator!=(const sorted_iterator&) const noexcept;

        sorted_iterator& operator++() noexcept;

        sorted_iterator operator++(int n);

        size_t size() const noexcept;

        void* operator*() const noexcept;

        bool occupied()const noexcept;

        sorted_iterator();

        sorted_iterator(void* trusted);
    };

    friend class sorted_iterator;
    friend class sorted_free_iterator;

    sorted_free_iterator free_begin() const noexcept;
    sorted_free_iterator free_end() const noexcept;

    sorted_iterator begin() const noexcept;
    sorted_iterator end() const noexcept;
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_SORTED_LIST_H