#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BOUNDARY_TAGS_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BOUNDARY_TAGS_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <mutex>
#include <sstream>

class allocator_boundary_tags final :
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

    void* _trusted_memory;
public:
    static constexpr size_t _meta_size = (sizeof(class logger*) + sizeof(allocator*) +
        sizeof(allocator_with_fit_mode::fit_mode) +
        sizeof(std::mutex) + sizeof(size_t) + sizeof(void*));
    static constexpr size_t _block_meta_size = sizeof(size_t) + 3 * sizeof(void*);

public:

    ~allocator_boundary_tags() override;

    allocator_boundary_tags(
        allocator_boundary_tags&& other) noexcept;

    allocator_boundary_tags& operator=(
        allocator_boundary_tags&& other) noexcept;

public:

    explicit allocator_boundary_tags(
        size_t space_size,
        allocator* parent_allocator = nullptr,
        logger* logger = nullptr,
        allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:

    [[nodiscard]] void* allocate(
        size_t value_size,
        size_t values_count) override;

    void deallocate(
        void* at) override;

public:

    inline void set_fit_mode(
        allocator_with_fit_mode::fit_mode mode) override;

private:

    inline allocator* get_allocator() const override;

public:

    std::vector<allocator_test_utils::block_info> get_blocks_info(size_t& full_free_size) const override;
    std::string get_dump(int* at, size_t size);
    std::string get_info_in_string() noexcept;

private:

    inline logger* get_logger() const override;

private:

    inline std::string get_typename() const noexcept override;

private:
   
    inline size_t get_size() const;

    inline std::mutex* get_mutex() const;

    inline allocator_with_fit_mode::fit_mode get_fit_mode() const;

    void* get_next_existing_block(void* left_side) const;

    void* allocate_first_fit(void*, size_t);
    void* allocate_best_fit(void* left_elem, size_t size);
    void* allocate_worst_fit(void* left_elem, size_t size);

    inline void* create_block_meta(void* block, size_t size, void* left, void* right);
    inline void* slide_block_for(void* block, size_t bytes) const;
    inline void* get_prev_existing_block(void* right_elem) const; 
    inline void** get_first_block_ptr() const noexcept;
    inline void* get_first_block() const noexcept;

    inline size_t get_block_data_size(void* right_elem) const;
    inline size_t get_block_distance(void*, void*) const;

    
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_BOUNDARY_TAGS_H