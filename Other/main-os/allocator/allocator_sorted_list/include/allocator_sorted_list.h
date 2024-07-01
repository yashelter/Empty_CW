#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_SORTED_LIST_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_SORTED_LIST_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <mutex>
#include <sstream>

class allocator_sorted_list final :
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

    void* _trusted_memory;

    static constexpr const size_t _meta_allocator = sizeof(logger*) + sizeof(allocator*) + 
                                                    sizeof(allocator_with_fit_mode::fit_mode) + 
                                                    sizeof(size_t) + sizeof(std::mutex) + sizeof(void*);
    static constexpr const size_t _meta_block = sizeof(size_t) + sizeof(void*);

public:

    ~allocator_sorted_list() override;

    allocator_sorted_list(allocator_sorted_list const& other) = delete;
    allocator_sorted_list& operator=(allocator_sorted_list const& other) = delete;

    allocator_sorted_list(allocator_sorted_list&& other) noexcept;
    allocator_sorted_list& operator=(allocator_sorted_list&& other) noexcept;

public:

    explicit allocator_sorted_list(
        size_t space_size,
        allocator* parent_allocator = nullptr,
        logger* logger = nullptr,
        allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:

    [[nodiscard]] void* allocate(
        size_t value_size,
        size_t values_count) override;

    void deallocate(void* at) override;

public:

    inline void set_fit_mode(allocator_with_fit_mode::fit_mode) override;

public:

    std::vector<allocator_test_utils::block_info> get_blocks_info(size_t& full_size_avail) const noexcept override;

private:

    [[nodiscard]] inline allocator* get_allocator() const override;
    [[nodiscard]] inline logger* get_logger() const override;
    [[nodiscard]] inline std::string get_typename() const noexcept override;

private:
    inline void fill_allocator_fields(size_t siz,
        allocator* parent_allocator,
        logger* logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode);

private:

    static inline void* slide(void*, size_t) noexcept;
    static inline size_t get_size_of_current_block(void*) noexcept;

    static std::string get_info_in_string(const std::vector<allocator_test_utils::block_info>&) noexcept;
    static std::string get_dump(char* at, size_t size);
private:
    

    inline std::mutex& get_mutex() noexcept;

    [[nodiscard]] inline allocator_with_fit_mode::fit_mode& get_fit_mode() const noexcept;

    [[nodiscard]] void* get_first_suitable(size_t)  const noexcept;
    [[nodiscard]] void* get_worst_suitable(size_t) const noexcept;
    [[nodiscard]] void* get_best_suitable(size_t)  const noexcept;

    [[nodiscard]] inline size_t get_allocator_full_space_size() const noexcept;


private:

    inline void** get_blocks_father(void*) const noexcept;

    static inline void** get_allocator_first_block(void*) noexcept;
    static inline void* get_ptr_from_block(void*) noexcept;
    inline void** get_ptr_from_previous_block(void*) const noexcept;
    inline void* get_previous_block_for_occured(void*) const noexcept;


    class free_blocks_iterator
    {
        void* _ptr_free;

    public:

        free_blocks_iterator();
        free_blocks_iterator(void* ptr);

        bool operator==(const free_blocks_iterator&) const noexcept;
        bool operator!=(const free_blocks_iterator&) const noexcept;

        free_blocks_iterator& operator++() noexcept;
        free_blocks_iterator operator++(int) noexcept;

        size_t size();

        void* get_ptr_free_block() const noexcept;
    };

    friend class free_blocks_iterator;

    [[nodiscard]] free_blocks_iterator get_begin() const noexcept;
    [[nodiscard]] free_blocks_iterator get_end() const noexcept;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_SORTED_LIST_H