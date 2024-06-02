#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <mutex>

class allocator_red_black_tree final:
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

    enum class block_color : unsigned char
    { RED, BLACK };

    struct block_data
    {
        bool occupied : 4;
        block_color color : 4;
    };

    void *_trusted_memory;

    static constexpr const size_t allocator_metadata_size = sizeof(logger*) + sizeof(allocator*) + sizeof(fit_mode) + sizeof(size_t) + sizeof(std::mutex) + sizeof(void*);
    static constexpr const size_t occupied_block_metadata_size = sizeof(block_data) + 3 * sizeof(void*);
    static constexpr const size_t free_block_metadata_size = sizeof(block_data) + 5 * sizeof(void*);

public:
    
    ~allocator_red_black_tree() override;
    
    allocator_red_black_tree(
        allocator_red_black_tree const &other) =delete;
    
    allocator_red_black_tree &operator=(
        allocator_red_black_tree const &other) =delete;
    
    allocator_red_black_tree(
        allocator_red_black_tree &&other) noexcept;
    
    allocator_red_black_tree &operator=(
        allocator_red_black_tree &&other) noexcept;

public:
    
    explicit allocator_red_black_tree(
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

    std::vector<allocator_test_utils::block_info> get_blocks_info() const override;
    
    inline void set_fit_mode(allocator_with_fit_mode::fit_mode mode) override;

private:

    inline std::string get_typename() const noexcept override;

    inline allocator *get_allocator() const override;

    inline logger *get_logger() const override;

    inline allocator_with_fit_mode::fit_mode& get_fit_mod() const noexcept;

    static inline size_t get_overall_size(void* trusted_memory) noexcept;

    inline std::mutex& get_mutex() const noexcept;

    static inline void** get_first_block_ptr(void* trusted_memory) noexcept;

    static bool is_block_occupied(void* block) noexcept;

    static size_t get_block_size(void* block, void* trusted) noexcept;

    std::vector<allocator_test_utils::block_info> get_blocks_info_inner() const override;

    static void*& get_forward_from_block(void* block) noexcept;

    static void*& get_back_from_block(void* block) noexcept;

    static block_data& get_data_from_block(void* block) noexcept;

    static void*& get_parent_from_block(void* block) noexcept;

    static void*& get_left_from_block(void* block) noexcept;

    static void*& get_right_from_block(void* block) noexcept;

    void insert(void* block) noexcept;

    void remove(void* block) noexcept;

	void print_tree(void* block, size_t depth = 0);

    static bool is_left_subtree(void* child, void* parent) noexcept;

    void small_right_rotation(void* rotatable) noexcept;
    void small_left_rotation(void* rotatable) noexcept;
    void big_right_rotation(void* rotatable) noexcept;
    void big_left_rotation(void* rotatable) noexcept;

    void reset_parent_ptr(void* block, void* new_ptr);

    void rebalance_black_leaf(void* parent, void* deleted = nullptr);

    void* get_first(size_t size) const noexcept;
    void* get_best(size_t size) const noexcept;
    void* get_worst(size_t size) const noexcept;

    size_t get_free_size() const noexcept;

    class rb_iterator
    {
        void* _block_ptr;
        void* _trusted;

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = void*;
        using reference = void*&;
        using pointer = void**;
        using difference_type = ptrdiff_t;

        bool operator==(const rb_iterator&) const noexcept;

        bool operator!=(const rb_iterator&) const noexcept;

        rb_iterator& operator++() noexcept;

        rb_iterator operator++(int n);

        size_t size() const noexcept;

        void* operator*() const noexcept;

        bool occupied()const noexcept;

        rb_iterator();

        rb_iterator(void* trusted);
    };

    friend class rb_iterator;

    rb_iterator begin() const noexcept;
    rb_iterator end() const noexcept;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H