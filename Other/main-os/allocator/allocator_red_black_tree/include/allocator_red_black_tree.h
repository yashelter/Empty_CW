#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H

#include <allocator_guardant.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <mutex>
#include <sstream>

using byte = unsigned char;

class allocator_red_black_tree final :
    private allocator_guardant,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

    enum class color : unsigned char
    {
        Red,
        Black
    };

    struct _tree_color
    {
        bool occupied : 4;
        color _color : 4;
    };

    void* _trusted_memory;

    static constexpr const size_t _meta_allocator = sizeof(logger*) + sizeof(allocator*) + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(size_t) + sizeof(std::mutex) + sizeof(void*);
    static constexpr const size_t _meta_free_block = sizeof(_tree_color) + 5 * sizeof(void*);
    static constexpr const size_t _meta_occup_block = sizeof(_tree_color) + 3 * sizeof(void*);


public:

    ~allocator_red_black_tree() override;

    allocator_red_black_tree(allocator_red_black_tree const& other) = delete;
    allocator_red_black_tree& operator=(allocator_red_black_tree const& other) = delete;

    allocator_red_black_tree(allocator_red_black_tree&& other) noexcept;
    allocator_red_black_tree& operator=(allocator_red_black_tree&& other) noexcept;

public:

    explicit allocator_red_black_tree(
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

    inline void set_fit_mode(allocator_with_fit_mode::fit_mode mode) override;


public:

    std::vector<allocator_test_utils::block_info> get_blocks_info(size_t& full_size_avail) const noexcept override;

private:

    inline allocator* get_allocator() const override;
    inline logger* get_logger() const override;
    inline std::string get_typename() const noexcept override;

private:
    static inline void* slide(void* mem, size_t siz) noexcept;
    inline void fill_allocator_fields(size_t size,
        allocator* parent_allocator,
        logger* logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode);

    inline std::mutex& get_mutex() const noexcept;

    inline allocator_with_fit_mode::fit_mode& get_fit_mode() const noexcept;

    static inline size_t get_size_full(void* trusted) noexcept;
    static inline size_t get_size_block(void* current_block, void* trusted_mem) noexcept;

    static void*& get_forward(void* current_block) noexcept;
    static void*& get_back(void* current_block) noexcept;

    static _tree_color& get_occup_color(void* current_block) noexcept;

    static void*& get_parent(void* current_block) noexcept;
    static void*& get_left(void* current_block) noexcept;
    static void*& get_right(void* current_block) noexcept;

    static inline void** get_first_block(void* trusted_mem) noexcept;

    static bool is_not_availiable(void* current_block) noexcept;

    void update_parent_ptr(void* current_block, void* new_parent) noexcept;

    static bool is_left_subtree(void* child, void* parent) noexcept;

    void small_right_rotate(void* joint) noexcept;
    void small_left_rotate(void* joint) noexcept;

    void big_right_rotate(void* joint) noexcept;
    void big_left_rotate(void* joint) noexcept;

    void print_tree(void* block, size_t depth = 0);

    void insert_rb_tree(void* current_block) noexcept;
    void remove_rb_tree(void* current_block) noexcept;

    void rebalance(void* parent, void* deleted = nullptr);

    void* get_first_suitable(size_t size) const noexcept;
    void* get_worst_suitable(size_t size) const noexcept;
    void* get_best_suitable(size_t size) const noexcept;

    class iterator
    {
        void* _crutch;
        void* _ptr;

    public:

        iterator();
        iterator(void* ptr);

        bool operator==(const iterator& oth) const noexcept;
        bool operator!=(const iterator& oth) const noexcept;

        iterator& operator++() noexcept;
        iterator operator++(int) noexcept;

        size_t size();
        void* get_ptr_free_block() const noexcept;
        bool is_not_availiable() const noexcept;

    };

    friend class iterator;

    iterator begin_iter() const noexcept;
    iterator end_iter() const noexcept;

    static std::string get_info_in_string(const std::vector<allocator_test_utils::block_info>& vec) noexcept;
    static std::string get_dump(char* at, size_t size);

    size_t get_all_free_size() const noexcept;



};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H