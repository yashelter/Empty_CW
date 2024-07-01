#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H

#include <list>
#include <stack>
#include <vector>
#include <memory>
#include <logger.h>
#include <logger_guardant.h>
#include <allocator.h>
#include <allocator_guardant.h>
#include <not_implemented.h>
#include <search_tree.h>
#include <stack>

template<
    typename tkey,
    typename tvalue>
class binary_search_tree :
    public search_tree<tkey, tvalue>
{

protected:

    struct node
    {

    public:

        tkey key;
        tvalue value;
        node* left_subtree;
        node* right_subtree;

    public:

        explicit node(
            tkey const& key,
            tvalue const& value);

        explicit node(
            tkey const& key,
            tvalue&& value);


        virtual ~node() = default;
    };

public:

    // region iterators definition

    struct iterator_data
    {

    public:

        unsigned int depth;
        tkey key;
        tvalue value;

    public:

        virtual void update(const node* n, unsigned int _depth);

        virtual iterator_data* clone();

        explicit iterator_data(
            unsigned int depth = 0,
            tkey const& key = tkey(),
            tvalue const& value = tvalue());

        virtual ~iterator_data() = default;
    };

    class prefix_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit prefix_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        prefix_iterator(const prefix_iterator& other);
        prefix_iterator(prefix_iterator&& other) noexcept = default;

        prefix_iterator& operator=(const prefix_iterator& other);
        prefix_iterator& operator=(prefix_iterator&& other) noexcept = default;

        virtual ~prefix_iterator() = default;

        bool operator==(prefix_iterator const& other) const noexcept;
        bool operator!=(prefix_iterator const& other) const noexcept;

        prefix_iterator& operator++();
        prefix_iterator const operator++(int not_used);

        iterator_data* operator*();
    };

    class prefix_const_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit prefix_const_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        prefix_const_iterator(const prefix_const_iterator& other);
        prefix_const_iterator(prefix_const_iterator&& other) noexcept = default;

        prefix_const_iterator& operator=(const prefix_const_iterator& other);
        prefix_const_iterator& operator=(prefix_const_iterator&& other) noexcept = default;

        virtual ~prefix_const_iterator() = default;

        bool operator==(prefix_const_iterator const& other) const noexcept;
        bool operator!=(prefix_const_iterator const& other) const noexcept;

        prefix_const_iterator& operator++();
        prefix_const_iterator const operator++(int not_used);

        iterator_data const* operator*() const;

    };

    class prefix_reverse_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit prefix_reverse_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        prefix_reverse_iterator(const prefix_reverse_iterator& other);
        prefix_reverse_iterator(prefix_reverse_iterator&& other) noexcept = default;

        prefix_reverse_iterator& operator=(const prefix_reverse_iterator& other);
        prefix_reverse_iterator& operator=(prefix_reverse_iterator&& other) noexcept = default;

        virtual ~prefix_reverse_iterator() = default;

        bool operator==(prefix_reverse_iterator const& other) const noexcept;
        bool operator!=(prefix_reverse_iterator const& other) const noexcept;

        prefix_reverse_iterator& operator++();
        prefix_reverse_iterator const operator++(int not_used);

        iterator_data* operator*() const;

    };

    class prefix_const_reverse_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit prefix_const_reverse_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        prefix_const_reverse_iterator(const prefix_const_reverse_iterator& other);
        prefix_const_reverse_iterator(prefix_const_reverse_iterator&& other) noexcept = default;

        prefix_const_reverse_iterator& operator=(const prefix_const_reverse_iterator& other);
        prefix_const_reverse_iterator& operator=(prefix_const_reverse_iterator&& other) noexcept = default;

        virtual ~prefix_const_reverse_iterator() = default;

        bool operator==(prefix_const_reverse_iterator const& other) const noexcept;
        bool operator!=(prefix_const_reverse_iterator const& other) const noexcept;

        prefix_const_reverse_iterator& operator++();
        prefix_const_reverse_iterator const operator++(int not_used);

        iterator_data const* operator*() const;

    };

    class infix_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        friend class binary_search_tree;

        explicit infix_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        infix_iterator(const infix_iterator& other);
        infix_iterator(infix_iterator&& other) noexcept = default;

        infix_iterator& operator=(const infix_iterator& other);
        infix_iterator& operator=(infix_iterator&& other) noexcept = default;

        virtual ~infix_iterator() = default;

        bool operator==(infix_iterator const& other) const noexcept;
        bool operator!=(infix_iterator const& other) const noexcept;

        infix_iterator& operator++();
        infix_iterator const operator++(int not_used);

        iterator_data* operator*();

    };

    class infix_const_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit infix_const_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        infix_const_iterator(const infix_const_iterator& other);
        infix_const_iterator(infix_const_iterator&& other) noexcept = default;

        infix_const_iterator& operator=(const infix_const_iterator& other);
        infix_const_iterator& operator=(infix_const_iterator&& other) noexcept = default;

        virtual ~infix_const_iterator() = default;

        bool operator==(infix_const_iterator const& other) const noexcept;
        bool operator!=(infix_const_iterator const& other) const noexcept;

        infix_const_iterator& operator++();
        infix_const_iterator operator++(
            int not_used);

        iterator_data const* operator*() const;

    };

    class infix_reverse_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit infix_reverse_iterator(const std::stack<node**>& path = std::stack<node**>(), 
                                            iterator_data* data = nullptr);

    public:

        infix_reverse_iterator(const infix_reverse_iterator& other);
        infix_reverse_iterator(infix_reverse_iterator&& other) noexcept = default;

        infix_reverse_iterator& operator=(const infix_reverse_iterator& other);
        infix_reverse_iterator& operator=(infix_reverse_iterator&& other) noexcept = default;

        virtual ~infix_reverse_iterator() = default;

        bool operator==(infix_reverse_iterator const& other) const noexcept;
        bool operator!=(infix_reverse_iterator const& other) const noexcept;

        infix_reverse_iterator& operator++();
        infix_reverse_iterator const operator++(int not_used);

        iterator_data* operator*() const;
    };

    class infix_const_reverse_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit infix_const_reverse_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        infix_const_reverse_iterator(const infix_const_reverse_iterator& other);
        infix_const_reverse_iterator(infix_const_reverse_iterator&& other) noexcept = default;

        infix_const_reverse_iterator& operator=(const infix_const_reverse_iterator& other);
        infix_const_reverse_iterator& operator=(infix_const_reverse_iterator&& other) noexcept = default;

        virtual ~infix_const_reverse_iterator() = default;

        bool operator==(infix_const_reverse_iterator const& other) const noexcept;
        bool operator!=(infix_const_reverse_iterator const& other) const noexcept;

        infix_const_reverse_iterator& operator++();
        infix_const_reverse_iterator const operator++(int not_used);

        iterator_data const* operator*() const;
    };

    class postfix_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit postfix_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        postfix_iterator(const postfix_iterator& other);
        postfix_iterator(postfix_iterator&& other) noexcept = default;

        postfix_iterator& operator=(const postfix_iterator& other);
        postfix_iterator& operator=(postfix_iterator&& other) noexcept = default;

        virtual ~postfix_iterator() = default;

        bool operator==(postfix_iterator const& other) const noexcept;
        bool operator!=(postfix_iterator const& other) const noexcept;

        postfix_iterator& operator++();
        postfix_iterator const operator++(int not_used);

        iterator_data* operator*() const;
    };

    class postfix_const_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit postfix_const_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        postfix_const_iterator(const postfix_const_iterator& other);
        postfix_const_iterator(postfix_const_iterator&& other) noexcept = default;

        postfix_const_iterator& operator=(const postfix_const_iterator& other);
        postfix_const_iterator& operator=(postfix_const_iterator&& other) noexcept = default;

        virtual ~postfix_const_iterator() = default;

        bool operator==(postfix_const_iterator const& other) const noexcept;
        bool operator!=(postfix_const_iterator const& other) const noexcept;

        postfix_const_iterator& operator++();
        postfix_const_iterator const operator++(
            int not_used);

        iterator_data const* operator*() const;
    };

    class postfix_reverse_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit postfix_reverse_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        postfix_reverse_iterator(const postfix_reverse_iterator& other);
        postfix_reverse_iterator(postfix_reverse_iterator&& other) noexcept = default;

        postfix_reverse_iterator& operator=(const postfix_reverse_iterator& other);
        postfix_reverse_iterator& operator=(postfix_reverse_iterator&& other) noexcept = default;

        virtual ~postfix_reverse_iterator() = default;

        bool operator==(postfix_reverse_iterator const& other) const noexcept;
        bool operator!=(postfix_reverse_iterator const& other) const noexcept;

        postfix_reverse_iterator& operator++();
        postfix_reverse_iterator const operator++(int not_used);

        iterator_data* operator*() const;
    };

    class postfix_const_reverse_iterator
    {
    protected:

        std::unique_ptr<iterator_data> _data;
        std::stack<node**> _path;

    public:

        explicit postfix_const_reverse_iterator(const std::stack<node**>& path = std::stack<node**>(), iterator_data* data = nullptr);

    public:

        postfix_const_reverse_iterator(const postfix_const_reverse_iterator& other);
        postfix_const_reverse_iterator(postfix_const_reverse_iterator&& other) noexcept = default;

        postfix_const_reverse_iterator& operator=(const postfix_const_reverse_iterator& other);
        postfix_const_reverse_iterator& operator=(postfix_const_reverse_iterator&& other) noexcept = default;

        virtual ~postfix_const_reverse_iterator() = default;

        bool operator==(postfix_const_reverse_iterator const& other) const noexcept;
        bool operator!=(postfix_const_reverse_iterator const& other) const noexcept;

        postfix_const_reverse_iterator& operator++();
        postfix_const_reverse_iterator const operator++(int not_used);

        iterator_data const* operator*() const;

    };

    friend class prefix_iterator;
    friend class prefix_const_iterator;
    friend class prefix_reverse_iterator;
    friend class prefix_const_reverse_iterator;

    friend class postfix_iterator;
    friend class postfix_const_iterator;
    friend class postfix_reverse_iterator;
    friend class postfix_const_reverse_iterator;

    friend class infix_iterator;
    friend class infix_const_iterator;
    friend class infix_reverse_iterator;
    friend class infix_const_reverse_iterator;

    // endregion iterators definition

public:

    // region target operations strategies definition

    enum class insertion_of_existent_key_attempt_strategy
    {
        update_value,
        throw_an_exception
    };

    enum class disposal_of_nonexistent_key_attempt_strategy
    {
        do_nothing,
        throw_an_exception
    };

    // endregion target operations strategies definition

    // region target operations associated exception types

    class insertion_of_existent_key_attempt_exception final :
        public std::logic_error
    {
    private:

        tkey _key;

    public:

        explicit insertion_of_existent_key_attempt_exception(tkey const& key);

        tkey const& get_key() const noexcept;

    };

    class obtaining_of_nonexistent_key_attempt_exception final :
        public std::logic_error
    {

    private:

        tkey _key;

    public:

        explicit obtaining_of_nonexistent_key_attempt_exception(tkey const& key);

        tkey const& get_key() const noexcept;

    };

    class disposal_of_nonexistent_key_attempt_exception final :
        public std::logic_error
    {

    private:

        tkey _key;

    public:

        explicit disposal_of_nonexistent_key_attempt_exception(tkey const& key);

        tkey const& get_key() const noexcept;
    };

    // endregion target operations associated exception types


protected:

    mutable node* _root;

    insertion_of_existent_key_attempt_strategy _insertion_stratagy;

    disposal_of_nonexistent_key_attempt_strategy _disposal_strategy;

public:

    explicit binary_search_tree(
        std::function<int(tkey const&, tkey const&)> comparer,
        allocator* allocator = nullptr,
        logger* logger = nullptr,
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy = binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::throw_an_exception,
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy = binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy::throw_an_exception);

public:

    binary_search_tree(binary_search_tree<tkey, tvalue> const& other);
    binary_search_tree(binary_search_tree<tkey, tvalue>&& other) noexcept;

    binary_search_tree<tkey, tvalue>& operator=(binary_search_tree<tkey, tvalue> const& other);
    binary_search_tree<tkey, tvalue>& operator=(binary_search_tree<tkey, tvalue>&& other) noexcept;

    ~binary_search_tree() override;

public:

    void insert(tkey const& key, tvalue const& value) final;

    void insert(tkey const& key, tvalue&& value) final;

    tvalue const& obtain(tkey const& key) final;

    std::vector<typename associative_container<tkey, tvalue>::key_value_pair> obtain_between(
        tkey const& lower_bound,
        tkey const& upper_bound,
        bool lower_bound_inclusive,
        bool upper_bound_inclusive) final;

    tvalue dispose(tkey const& key) final;

public:

    void set_insertion_strategy(typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept;

    void set_removal_strategy(typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept;

public:

    // region iterators requests definition

    prefix_iterator begin_prefix() noexcept;
    prefix_iterator end_prefix() noexcept;
    prefix_const_iterator cbegin_prefix() const noexcept;
    prefix_const_iterator cend_prefix() const noexcept;
    prefix_reverse_iterator rbegin_prefix() noexcept;
    prefix_reverse_iterator rend_prefix() noexcept;
    prefix_const_reverse_iterator crbegin_prefix() const noexcept;
    prefix_const_reverse_iterator crend_prefix() const noexcept;

    infix_iterator begin_infix() noexcept;
    infix_iterator end_infix() noexcept;
    infix_const_iterator cbegin_infix() const noexcept;
    infix_const_iterator cend_infix() const noexcept;
    infix_reverse_iterator rbegin_infix() noexcept;
    infix_reverse_iterator rend_infix() noexcept;
    infix_const_reverse_iterator crbegin_infix() const noexcept;
    infix_const_reverse_iterator crend_infix() const noexcept;

    postfix_iterator begin_postfix() noexcept;
    postfix_iterator end_postfix() noexcept;
    postfix_const_iterator cbegin_postfix() const noexcept;
    postfix_const_iterator cend_postfix() const noexcept;
    postfix_reverse_iterator rbegin_postfix() noexcept;
    postfix_reverse_iterator rend_postfix() noexcept;
    postfix_const_reverse_iterator crbegin_postfix() const noexcept;
    postfix_const_reverse_iterator crend_postfix() const noexcept;

    // endregion iterators requests definition

protected:

    std::stack<node**> find_path(const tkey&);

    virtual void insert_inner(std::stack<node**>& node_path, const tkey& key, tvalue&& val);
    virtual void insert_inner(std::stack<node**>& node_path, const tkey& key, const tvalue& val);

    virtual tvalue& obtain_inner(std::stack<node**>& node_path);
    virtual tvalue dispose_inner(std::stack<node**>& node_path);

    void destroy_subtree(node* n) noexcept;

    virtual void copy_subtree(node** target, node* src);
    virtual void actions_after_update_insert(std::stack<node**>& path);

    static bool is_left_subtree(node* child, node* parent) noexcept;

    static node** get_last_node(infix_iterator& it);

    // region subtree rotations definition

    void small_left_rotation(typename binary_search_tree<tkey, tvalue>::node*& subtree_root) const;
    void small_right_rotation(typename binary_search_tree<tkey, tvalue>::node*& subtree_root) const;

    void big_left_rotation(typename binary_search_tree<tkey, tvalue>::node*& subtree_root) const;
    void big_right_rotation(typename binary_search_tree<tkey, tvalue>::node*& subtree_root) const;

    void double_left_rotation(typename binary_search_tree<tkey, tvalue>::node*& subtree_root) const;
    void double_right_rotation(typename binary_search_tree<tkey, tvalue>::node*& subtree_root) const;

    // endregion subtree rotations definition
};

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::node** binary_search_tree<tkey, tvalue>::get_last_node(binary_search_tree::infix_iterator& it)
{
    return it._path.top();
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::actions_after_update_insert(std::stack<node**>& path) {}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::is_left_subtree(binary_search_tree::node* child,
    binary_search_tree::node* parent) noexcept
{
    return child == parent->left_subtree;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::copy_subtree(binary_search_tree::node** target, binary_search_tree::node* src)
{
    if (src == nullptr)
    {
        *target = nullptr;
        return;
    }

    *target = reinterpret_cast<node*>(allocator_guardant::allocate_with_guard(sizeof(node)));
    try
    {
        allocator::construct(*target, *src);
    } catch (...)
    {
        allocator_guardant::deallocate_with_guard(*target);
        *target = nullptr;
        throw;
    }

    (*target)->right_subtree = nullptr;
    (*target)->left_subtree = nullptr;

    copy_subtree(&((*target)->left_subtree), src->left_subtree);
    copy_subtree(&((*target)->right_subtree), src->right_subtree);
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::destroy_subtree(binary_search_tree::node* n) noexcept
{
    if (n != nullptr)
    {
        destroy_subtree(n->left_subtree);
        destroy_subtree(n->right_subtree);

        allocator::destruct(n);
        allocator_guardant::deallocate_with_guard(n);
    }
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::insert_inner(std::stack<node**>& node_path, const tkey& key, tvalue&& val)
{
    *node_path.top() = reinterpret_cast<node*>(allocator_guardant::allocate_with_guard(sizeof(node)));
    try
    {
        allocator::construct(*node_path.top(), key, std::move(val));
    } catch (...)
    {
        allocator_guardant::deallocate_with_guard(*node_path.top());
        *node_path.top() = nullptr;
        throw;
    }
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::insert_inner(std::stack<node**>& node_path, const tkey& key, const tvalue& val)
{
    *node_path.top() = static_cast<node*>(allocator_guardant::allocate_with_guard(sizeof(node)));
    try
    {
        allocator::construct(*node_path.top(), key, val);
    } catch (...)
    {
        allocator_guardant::deallocate_with_guard(*node_path.top());
        *node_path.top() = nullptr;
        throw;
    }
}

template<typename tkey, typename tvalue>
tvalue& binary_search_tree<tkey, tvalue>::obtain_inner(std::stack<node**>& node_path)
{
    return (*node_path.top())->value;
}

template<typename tkey, typename tvalue>
tvalue binary_search_tree<tkey, tvalue>::dispose_inner(std::stack<node**>& node_path)
{
    tvalue res = (*node_path.top())->value;

    node* current_node = *node_path.top();

    if (current_node->right_subtree == nullptr && current_node->left_subtree == nullptr)
    {
        *node_path.top() = nullptr;
    } else if (current_node->right_subtree == nullptr || current_node->left_subtree == nullptr)
    {
        node* new_old_node = current_node->right_subtree != nullptr ? 
                current_node->right_subtree : 
                current_node->left_subtree;

        *node_path.top() = new_old_node;
    } else
    {
        typename binary_search_tree<tkey, tvalue>::node** new_subtree = &current_node->left_subtree;

        while ((*new_subtree)->right_subtree != nullptr)
        {
            new_subtree = &((*new_subtree)->right_subtree);
        }

        typename binary_search_tree<tkey, tvalue>::node* tmp = *new_subtree;

        *new_subtree = (*new_subtree)->left_subtree;
        *node_path.top() = tmp;

        tmp->left_subtree = current_node->left_subtree == tmp ?
                                                          tmp->left_subtree : 
                                                          current_node->left_subtree;
        tmp->right_subtree = current_node->right_subtree;
    }

    allocator::destruct(current_node);
    allocator_guardant::deallocate_with_guard(current_node);

    return res;
}

template<typename tkey, typename tvalue>
std::stack<typename binary_search_tree<tkey, tvalue>::node**> binary_search_tree<tkey, tvalue>::find_path(const tkey& key)
{
    std::stack<node**> result;

    result.push(&_root);
    node** current_node = &_root;

    while (*current_node != nullptr)
    {
        int res = search_tree<tkey, tvalue>::_keys_comparer(key, (*current_node)->key);

        if (res == 0)
        {
            break;
        }
        else if (res > 0)
        {
            current_node = &((*current_node)->right_subtree);
        }
        else
        {
            current_node = &((*current_node)->left_subtree);
        }

        result.push(current_node);
    }

    return result;
}

// region binary_search_tree<tkey, tvalue>::node methods implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::node::node(tkey const& key_, tvalue const& value_) : key(key_), value(value_), left_subtree(nullptr), right_subtree(nullptr) {}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::node::node(tkey const& key_, tvalue&& value_) : key(key_), value(std::move(value_)), left_subtree(nullptr), right_subtree(nullptr) {}

// endregion binary_search_tree<tkey, tvalue>::node methods implementation

// region iterators implementation

// region iterator data implementation

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data* binary_search_tree<tkey, tvalue>::iterator_data::clone()
{
    return new iterator_data(*this);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::iterator_data::iterator_data(unsigned int depth, tkey const& key, tvalue const& value) :
    depth(depth), key(key), value(value)
{
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::iterator_data::update(const binary_search_tree::node* n, unsigned int _depth)
{
    key = n->key;
    value = n->value;
    depth = _depth;
}
// endregion iterator data implementation

// region prefix_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_iterator::prefix_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_iterator::prefix_iterator(const prefix_iterator& other) : 
    _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator& binary_search_tree<tkey, tvalue>::prefix_iterator::operator=(const binary_search_tree::prefix_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_iterator::prefix_const_iterator(const prefix_const_iterator& other) : _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator& binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator=(const binary_search_tree::prefix_const_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::prefix_reverse_iterator(const prefix_reverse_iterator& other) : _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator& binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator=(const binary_search_tree::prefix_reverse_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(const prefix_const_reverse_iterator& other) :
    _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator& binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator=(const binary_search_tree::prefix_const_reverse_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::infix_iterator(const infix_iterator& other) :
    _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator& binary_search_tree<tkey, tvalue>::infix_iterator::operator=(const binary_search_tree::infix_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_iterator::infix_const_iterator(const infix_const_iterator& other) : 
        _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator& binary_search_tree<tkey, tvalue>::infix_const_iterator::operator=(const binary_search_tree::infix_const_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::infix_reverse_iterator(const infix_reverse_iterator& other) : _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator& binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator=(const binary_search_tree::infix_reverse_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::infix_const_reverse_iterator(const infix_const_reverse_iterator& other) : _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator& binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator=(const binary_search_tree::infix_const_reverse_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_iterator::postfix_iterator(const postfix_iterator& other) : _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator& binary_search_tree<tkey, tvalue>::postfix_iterator::operator=(const binary_search_tree::postfix_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_iterator::postfix_const_iterator(const postfix_const_iterator& other) : _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator& binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator=(const binary_search_tree::postfix_const_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::postfix_reverse_iterator(const postfix_reverse_iterator& other) : _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator& binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator=(const binary_search_tree::postfix_reverse_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template <typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(const postfix_const_reverse_iterator& other) : _path(other._path), _data(other._data.get() == nullptr ? nullptr : other._data->clone()) {}

template <typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator& binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator=(const binary_search_tree::postfix_const_reverse_iterator& other)
{
    if (*this != other)
    {
        _path = other._path;
        _data = std::unique_ptr<iterator_data>(other._data.get() == nullptr ? nullptr : other._data->clone());
    }
    return *this;
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::prefix_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) ||
           (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::prefix_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator& binary_search_tree<tkey, tvalue>::prefix_iterator::operator++()
{
	if (_path.empty())
	{
		return *this;
	}
    if ((*_path.top())->left_subtree != nullptr)
    {
        _path.push(&(*_path.top())->left_subtree);
    }
    else if ((*_path.top())->right_subtree != nullptr)
    {
        _path.push(&(*_path.top())->right_subtree);
    }
    else
    {
        node* current_node = *_path.top();
        _path.pop();
        bool from_left = is_left_subtree(current_node, *_path.top());

        while (!_path.empty() && (!from_left || (*_path.top())->right_subtree != nullptr))
        {
            current_node = *_path.top();
            _path.pop();
            if (!_path.empty())
            {
                from_left = is_left_subtree(current_node, *_path.top());
            }
        }

        if (!_path.empty())
        {
            _path.push(&((*_path.top())->right_subtree));
        }

    }

    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator const binary_search_tree<tkey, tvalue>::prefix_iterator::operator++(int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data* binary_search_tree<tkey, tvalue>::prefix_iterator::operator*()
{
    return _data.get();
}

// endregion prefix_iterator implementation

// region prefix_const_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_iterator::prefix_const_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::prefix_const_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::prefix_const_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator& binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator++()
{
    if (_path.empty())
    {
        return *this;
    }
    if ((*_path.top())->left_subtree != nullptr)
    {
        _path.push(&(*_path.top())->left_subtree);
    }
    else if ((*_path.top())->right_subtree != nullptr)
    {
        _path.push(&(*_path.top())->right_subtree);
    }
    else
    {
        node* current_node = *_path.top();
        _path.pop();
        bool from_left = is_left_subtree(current_node, *_path.top());

        while (!_path.empty() && (!from_left || (*_path.top())->right_subtree == nullptr))
        {
            current_node = *_path.top();
            _path.pop();
            if (!_path.empty())
            {
                from_left = is_left_subtree(current_node, *_path.top());
            }
        }

        if (!_path.empty())
        {
            _path.push(&((*_path.top())->right_subtree));
        }

    }

    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator const binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator++(int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const* binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator*() const
{
    return _data.get();
}

// endregion prefix_const_iterator implementation

// region prefix_reverse_iterator implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::prefix_reverse_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator& binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator++()
{
    if (_path.empty())
    {
        return *this;
    }
    if ((*_path.top())->right_subtree != nullptr)
    {
        _path.push(&(*_path.top())->right_subtree);
    }
    else if ((*_path.top())->left_subtree != nullptr)
    {
        _path.push(&(*_path.top())->left_subtree);
    }
    else
    {
        node* current_node = *_path.top();
        _path.pop();
        bool from_left = is_left_subtree(current_node, *_path.top());

        while (!_path.empty() && (from_left || (*_path.top())->left_subtree != nullptr))
        {
            current_node = *_path.top();
            _path.pop();
            if (!_path.empty())
            {
                from_left = is_left_subtree(current_node, *_path.top());
            }
        }

        if (!_path.empty())
        {
            _path.push(&((*_path.top())->left_subtree));
        }

    }

    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator const binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator++(
    int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data* binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator*() const
{
    return _data.get();
}

// endregion prefix_reverse_iterator implementation

// region prefix_const_reverse_iterator implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator& binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator++()
{
    if (_path.empty())
    {
        return *this;
    }
    if ((*_path.top())->right_subtree != nullptr)
    {
        _path.push(&(*_path.top())->right_subtree);
    }
    else if ((*_path.top())->left_subtree != nullptr)
    {
        _path.push(&(*_path.top())->left_subtree);
    }
    else
    {
        node* current_node = *_path.top();
        _path.pop();
        bool from_left = is_left_subtree(current_node, *_path.top());

        while (!_path.empty() && (from_left || (*_path.top())->left_subtree != nullptr))
        {
            current_node = *_path.top();
            _path.pop();
            if (!_path.empty())
            {
                from_left = is_left_subtree(current_node, *_path.top());
            }
        }

        if (!_path.empty())
            _path.push(&((*_path.top())->left_subtree));

    }

    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator const binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator++(
    int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const* binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator*() const
{
    return _data.get();
}

// endregion prefix_const_reverse_iterator implementation

// region infix_iterator implementation

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::infix_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::infix_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<typename tkey, typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::infix_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator& binary_search_tree<tkey, tvalue>::infix_iterator::operator++()
{

    if ((*_path.top())->right_subtree != nullptr)
    {
        _path.push(&((*_path.top())->right_subtree));

        while ((*_path.top())->left_subtree != nullptr)
        {
            _path.push(&((*_path.top())->left_subtree));
        }
    } else
    {
        node* current_node = *_path.top();
        _path.pop();
        bool is_left = false;
		if (!_path.empty())
		{
			is_left = is_left_subtree(current_node, *_path.top());
		}

        while (!is_left && !_path.empty())
        {
            current_node = *_path.top();
            _path.pop();
            if (!_path.empty())
            {
                is_left = is_left_subtree(current_node, *_path.top());
            }
        }
    }

    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator const binary_search_tree<tkey, tvalue>::infix_iterator::operator++(int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data* binary_search_tree<tkey, tvalue>::infix_iterator::operator*()
{
    return _data.get();
}

// endregion infix_iterator implementation

// region infix_const_iterator implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_iterator::infix_const_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::infix_const_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::infix_const_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator& binary_search_tree<tkey, tvalue>::infix_const_iterator::operator++()
{
    if ((*_path.top())->right_subtree != nullptr)
    {
        _path.push(&((*_path.top())->right_subtree));

        while ((*_path.top())->left_subtree != nullptr)
        {
            _path.push(&((*_path.top())->left_subtree));
        }
    } else
    {
        node* current_node = *_path.top();
        _path.pop();
        bool is_left;
        if (!_path.empty()){
            is_left = is_left_subtree(current_node, *_path.top());
        }

        while (!is_left && !_path.empty())
        {
            current_node = *_path.top();
            _path.pop();
            if (!_path.empty())
            {
                is_left = is_left_subtree(current_node, *_path.top());
            }
        }
    }

    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator binary_search_tree<tkey, tvalue>::infix_const_iterator::operator++(
    int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const* binary_search_tree<tkey, tvalue>::infix_const_iterator::operator*() const
{
    return _data.get();
}

// endregion infix_const_iterator implementation

// region infix_reverse_iterator implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::infix_reverse_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator& binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator++()
{
    if ((*_path.top())->left_subtree != nullptr)
    {
        _path.push(&((*_path.top())->left_subtree));

        while ((*_path.top())->right_subtree != nullptr)
        {
            _path.push(&((*_path.top())->right_subtree));
        }
    } else
    {
        node* current_node = *_path.top();
        _path.pop();
        bool is_left = false;
		if (!_path.empty())
		{
			is_left = is_left_subtree(current_node, *_path.top());
		}

        while (is_left && !_path.empty())
        {
            current_node = *_path.top();
            _path.pop();
            if (!_path.empty())
            {
                is_left = is_left_subtree(current_node, *_path.top());
            }
        }
    }

    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator const binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator++(
    int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data* binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator*() const
{
    return _data.get();
}

// endregion infix_reverse_iterator implementation

// region infix_const_reverse_iterator implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::infix_const_reverse_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator& binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator++()
{
    if ((*_path.top())->left_subtree != nullptr)
    {
        _path.push(&((*_path.top())->left_subtree));

        while ((*_path.top())->right_subtree != nullptr)
        {
            _path.push(&((*_path.top())->right_subtree));
        }
    } else
    {
        node* current_node = *_path.top();
        _path.pop();
        bool is_left = false;
        if (!_path.empty())
            is_left = is_left_subtree(current_node, *_path.top());

        while (is_left && !_path.empty())
        {
            current_node = *_path.top();
            _path.pop();
            if (!_path.empty())
                is_left = is_left_subtree(current_node, *_path.top());
        }
    }

    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator const binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator++(
    int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const* binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator*() const
{
    return _data.get();
}

// endregion infix_const_reverse_iterator implementation

// region postfix_iterator implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_iterator::postfix_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::postfix_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::postfix_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator& binary_search_tree<tkey, tvalue>::postfix_iterator::operator++()
{
    node* current_node = *_path.top();
    _path.pop();

    if (!_path.empty())
    {
        bool from_left = is_left_subtree(current_node, *_path.top());

        if (from_left)
        {
            if ((*_path.top())->right_subtree != nullptr)
            {
                _path.push(&((*_path.top())->right_subtree));

                while ((*_path.top())->left_subtree != nullptr)
                {
                    _path.push(&((*_path.top())->left_subtree));
                }
            }
        }

        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator const binary_search_tree<tkey, tvalue>::postfix_iterator::operator++(
    int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data* binary_search_tree<tkey, tvalue>::postfix_iterator::operator*() const
{
    return _data.get();
}

// endregion postfix_iterator implementation

// region postfix_const_iterator implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_iterator::postfix_const_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::postfix_const_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::postfix_const_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator& binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator++()
{
    node* current_node = *_path.top();
    _path.pop();

    if (!_path.empty())
    {
        bool from_left = is_left_subtree(current_node, *_path.top());

        if (from_left)
        {
            if ((*_path.top())->right_subtree != nullptr)
            {
                _path.push(&((*_path.top())->right_subtree));

                while ((*_path.top())->left_subtree != nullptr)
                {
                    _path.push(&((*_path.top())->left_subtree));
                }
            }
        }

        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator const binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator++(
    int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const* binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator*() const
{
    return _data.get();
}

// endregion postfix_const_iterator implementation

// region postfix_reverse_iterator implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::postfix_reverse_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator& binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator++()
{
    node* current_node = *_path.top();
    _path.pop();

    if (!_path.empty())
    {
        bool from_left = is_left_subtree(current_node, *_path.top());

        if (!from_left)
        {
            if ((*_path.top())->left_subtree != nullptr)
            {
                _path.push(&((*_path.top())->left_subtree));

                while ((*_path.top())->right_subtree != nullptr)
                {
                    _path.push(&((*_path.top())->right_subtree));
                }
            }
        }

        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator const binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator++(
    int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data* binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator*() const
{
    return _data.get();
}

// endregion postfix_reverse_iterator implementation

// region postfix_const_reverse_iterator implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(const std::stack<node**>& path, iterator_data* data) : _path(path), _data(data)
{
    if (!_path.empty())
    {
        _data->update(*_path.top(), _path.size() - 1);
    }
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator==(
    typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator const& other) const noexcept
{
    return (_path.empty() && other._path.empty()) || (!_path.empty() && !other._path.empty() && _path.top() == other._path.top());
}

template<
    typename tkey,
    typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator!=(
    typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator const& other) const noexcept
{
    return !(*this == other);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator& binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator++()
{
    node* current_node = *_path.top();
    _path.pop();

    if (!_path.empty())
    {
        bool from_left = is_left_subtree(current_node, *_path.top());

        if (!from_left)
        {
            if ((*_path.top())->left_subtree != nullptr)
            {
                _path.push(&((*_path.top())->left_subtree));

                while ((*_path.top())->right_subtree != nullptr)
                {
                    _path.push(&((*_path.top())->right_subtree));
                }
            }
        }

        _data->update(*_path.top(), _path.size() - 1);
    }

    return *this;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator const binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator++(
    int not_used)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data const* binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator*() const
{
    return _data.get();
}

// endregion postfix_const_reverse_iterator implementation

// endregion iterators implementation

// region target operations associated exception types implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_exception::insertion_of_existent_key_attempt_exception(
    tkey const& key) :
    std::logic_error("Attempt to insert already existing key inside the tree."), _key(key)
{

}

template<
    typename tkey,
    typename tvalue>
tkey const& binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_exception::get_key() const noexcept
{
    return _key;
}

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::obtaining_of_nonexistent_key_attempt_exception::obtaining_of_nonexistent_key_attempt_exception(
    tkey const& key) :
    std::logic_error("Attempt to obtain a value by non-existing key from the tree."), _key(key)
{
}

template<
    typename tkey,
    typename tvalue>
tkey const& binary_search_tree<tkey, tvalue>::obtaining_of_nonexistent_key_attempt_exception::get_key() const noexcept
{
    return _key;
}

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_exception::disposal_of_nonexistent_key_attempt_exception(
    tkey const& key) :
    std::logic_error("Attempt to dispose a value by non-existing key from the tree."), _key(key)
{
}

template<
    typename tkey,
    typename tvalue>
tkey const& binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_exception::get_key() const noexcept
{
    return _key;
}

// endregion target operations associated exception types implementation

// region construction, assignment, destruction implementation

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(
    std::function<int(tkey const&, tkey const&)> keys_comparer,
    allocator* allocator,
    logger* logger,
    typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy,
    typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) :
    search_tree<tkey, tvalue>(keys_comparer, logger, allocator), _insertion_stratagy(insertion_strategy), _disposal_strategy(disposal_strategy), _root(nullptr) {}

template<
    typename tkey,
    typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(
    binary_search_tree<tkey, tvalue> const& other) : search_tree<tkey, tvalue>(other), _insertion_stratagy(other._insertion_stratagy), _disposal_strategy(other._disposal_strategy)
{
    try
    {
        other.copy_subtree(&_root, other._root);
    } catch (...)
    {
        destroy_subtree(_root);
        throw;
    }
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(
    binary_search_tree<tkey, tvalue>&& other) noexcept : search_tree<tkey, tvalue>(std::move(other)), _insertion_stratagy(other._insertion_stratagy), _disposal_strategy(other._disposal_strategy)
{
    _root = std::exchange(other._root, nullptr);
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>& binary_search_tree<tkey, tvalue>::operator=(
    binary_search_tree<tkey, tvalue> const& other)
{
    if (this != &other)
    {
        search_tree<tkey, tvalue>::operator=(other);
        _insertion_stratagy = other._insertion_stratagy;
        _disposal_strategy = other._disposal_strategy;

        try
        {
            copy_subtree(&_root, other._root);
        } catch (...)
        {
            destroy_subtree(_root);
            throw;
        }
    }
    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>& binary_search_tree<tkey, tvalue>::operator=(
    binary_search_tree<tkey, tvalue>&& other) noexcept
{
    if (this != &other)
    {
        search_tree<tkey, tvalue>::operator=(std::move(other));
        _insertion_stratagy = other._insertion_stratagy;
        _disposal_strategy = other._disposal_strategy;

        std::swap(_root, other._root);
    }
    return *this;
}

template<typename tkey, typename tvalue>
binary_search_tree<tkey, tvalue>::~binary_search_tree()
{
    destroy_subtree(_root);
}

// endregion construction, assignment, destruction implementation

// region associative_container<tkey, tvalue> contract implementation

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::insert(tkey const& key, tvalue const& value)
{
    auto path = find_path(key);

    if (*path.top() != nullptr)
    {
        if (_insertion_stratagy == insertion_of_existent_key_attempt_strategy::throw_an_exception)
        {
            throw insertion_of_existent_key_attempt_exception(key);
        } else
        {
            (*path.top())->value = value;
            actions_after_update_insert(path);
        }
        return;
    }

    insert_inner(path, key, value);
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::insert(tkey const& key, tvalue&& value)
{
    auto path = find_path(key);

    if (*path.top() != nullptr)
    {
        if (_insertion_stratagy == insertion_of_existent_key_attempt_strategy::throw_an_exception)
        {
            throw insertion_of_existent_key_attempt_exception(key);
        } else
        {
            (*path.top())->value = std::move(value);
            actions_after_update_insert(path);
        }
        return;
    }

    insert_inner(path, key, std::move(value));
}

template<typename tkey, typename tvalue>
tvalue const& binary_search_tree<tkey, tvalue>::obtain(tkey const& key)
{
    auto path = find_path(key);

    if (*path.top() == nullptr)
    {
        throw obtaining_of_nonexistent_key_attempt_exception(key);
    }

    return obtain_inner(path);
}

template<
    typename tkey,
    typename tvalue>
std::vector<typename associative_container<tkey, tvalue>::key_value_pair> binary_search_tree<tkey, tvalue>::obtain_between(
    tkey const& lower_bound,
    tkey const& upper_bound,
    bool lower_bound_inclusive,
    bool upper_bound_inclusive)
{
    auto path = find_path(lower_bound);

    if (*path.top() == nullptr)
    {
        path.pop();
    }

    infix_iterator it(path, new iterator_data()), end = end_infix();
    std::vector<typename associative_container<tkey, tvalue>::key_value_pair> result;

    if (it == end)
    {
        return result;
    }

	int cmpresult = search_tree<tkey, tvalue>::_keys_comparer((*it)->key, lower_bound);
	if (cmpresult < 0)
	{
		++it;
	} else if (cmpresult == 0)
	{
		if (lower_bound_inclusive)
		{
			result.emplace_back((*it)->key, (*it)->value);
		}
		++it;
	}

	if (it != end)
	{
		cmpresult = search_tree<tkey, tvalue>::_keys_comparer((*it)->key, upper_bound);
	}

	while (it != end && cmpresult < 0)
	{
		result.emplace_back((*it)->key, (*it)->value);
		++it;
		if (it != end)
		{
			cmpresult = search_tree<tkey, tvalue>::_keys_comparer((*it)->key, upper_bound);
		}
	}

	if (it != end && cmpresult == 0 && upper_bound_inclusive)
	{
		result.emplace_back((*it)->key, (*it)->value);
	}


    return result;
}

template<
    typename tkey,
    typename tvalue>
tvalue binary_search_tree<tkey, tvalue>::dispose(
    tkey const& key)
{
    auto path = find_path(key);

    if (*path.top() == nullptr)
    {
        if (_disposal_strategy == disposal_of_nonexistent_key_attempt_strategy::throw_an_exception)
        {
            throw disposal_of_nonexistent_key_attempt_exception(key);
        }
           
        else
        {
            if constexpr (std::is_default_constructible<tvalue>::value)
            {
                return tvalue();
            }
            else
            {
                throw disposal_of_nonexistent_key_attempt_exception(key);
            }
        }
    }

    return dispose_inner(path);
}

// endregion associative_containers contract implementations

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::set_insertion_strategy(
    typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept
{
    _insertion_stratagy = insertion_strategy;
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::set_removal_strategy(
    typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept
{
    _disposal_strategy = disposal_strategy;
}

// region iterators requesting implementation

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator binary_search_tree<tkey, tvalue>::begin_prefix() noexcept
{
    std::stack<node**> stack;
    if (_root != nullptr)
        stack.push(&_root);

    return prefix_iterator(stack, new iterator_data);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator binary_search_tree<tkey, tvalue>::end_prefix() noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator binary_search_tree<tkey, tvalue>::cbegin_prefix() const noexcept
{
    std::stack<node**> stack;
    if (_root != nullptr)
    {
        stack.push(&_root);
    }
    return prefix_const_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator binary_search_tree<tkey, tvalue>::cend_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_const_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator binary_search_tree<tkey, tvalue>::rbegin_prefix() noexcept
{
    std::stack<node**> stack;
    if (_root != nullptr)
    {
        stack.push(&_root);
    }

    return prefix_reverse_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator binary_search_tree<tkey, tvalue>::rend_prefix() noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_reverse_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crbegin_prefix() const noexcept
{
    std::stack<node**> stack;
    if (_root != nullptr)
    {
        stack.push(&_root);
    }

    return prefix_const_reverse_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crend_prefix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator(std::stack<node**>(), nullptr);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator binary_search_tree<tkey, tvalue>::begin_infix() noexcept
{
    std::stack<node**> stack;
    node** current_node = &_root;
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = &((*current_node)->left_subtree);
    }
    return infix_iterator(stack, new iterator_data);
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator binary_search_tree<tkey, tvalue>::end_infix() noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator binary_search_tree<tkey, tvalue>::cbegin_infix() const noexcept
{
    std::stack<node**> stack;
    node** current_node = &_root;
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = &((*current_node)->left_subtree);
    }
    return infix_const_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator binary_search_tree<tkey, tvalue>::cend_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_const_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator binary_search_tree<tkey, tvalue>::rbegin_infix() noexcept
{
    std::stack<node**> stack;
    node** current_node = &_root;
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = &((*current_node)->right_subtree);
    }
    return infix_reverse_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator binary_search_tree<tkey, tvalue>::rend_infix() noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_reverse_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crbegin_infix() const noexcept
{
    std::stack<node**> stack;
    node** current_node = &_root;
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = &((*current_node)->right_subtree);
    }
    return infix_const_reverse_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crend_infix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator binary_search_tree<tkey, tvalue>::begin_postfix() noexcept
{
    std::stack<node**> stack;
    node** current_node = &_root;
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = (*current_node)->left_subtree != nullptr ? &((*current_node)->left_subtree) : &((*current_node)->right_subtree);
    }
    return postfix_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator binary_search_tree<tkey, tvalue>::end_postfix() noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator binary_search_tree<tkey, tvalue>::cbegin_postfix() const noexcept
{
    std::stack<node**> stack;
    node** current_node = &_root;
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = (*current_node)->left_subtree != nullptr ? &((*current_node)->left_subtree) : &((*current_node)->right_subtree);
    }
    return postfix_const_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator binary_search_tree<tkey, tvalue>::cend_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_const_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator binary_search_tree<tkey, tvalue>::rbegin_postfix() noexcept
{
    std::stack<node**> stack;
    node** current_node = &_root;
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = (*current_node)->right_subtree != nullptr ? &((*current_node)->right_subtree) : &((*current_node)->left_subtree);
    }
    return postfix_reverse_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator binary_search_tree<tkey, tvalue>::rend_postfix() noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_reverse_iterator(std::stack<node**>(), nullptr);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crbegin_postfix() const noexcept
{
    std::stack<node**> stack;
    node** current_node = &_root;
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = (*current_node)->right_subtree != nullptr ? &((*current_node)->right_subtree) : &((*current_node)->left_subtree);
    }
    return postfix_const_reverse_iterator(stack, new iterator_data);
}

template<
    typename tkey,
    typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator binary_search_tree<tkey, tvalue>::crend_postfix() const noexcept
{
    return binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator(std::stack<node**>(), nullptr);
}


// endregion iterators request implementation

// region subtree rotations implementation

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::small_left_rotation(
    binary_search_tree<tkey, tvalue>::node*& subtree_root) const
{
    if (subtree_root != nullptr && subtree_root->right_subtree != nullptr)
    {
        node* tmp = subtree_root;

        subtree_root = tmp->right_subtree;
        tmp->right_subtree = subtree_root->left_subtree;
        subtree_root->left_subtree = tmp;
    }
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::small_right_rotation(
    binary_search_tree<tkey, tvalue>::node*& subtree_root) const
{
    if (subtree_root != nullptr && subtree_root->left_subtree != nullptr)
    {
        node* tmp = subtree_root;

        subtree_root = tmp->left_subtree;
        tmp->left_subtree = subtree_root->right_subtree;
        subtree_root->right_subtree = tmp;
    }
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::big_left_rotation(
    binary_search_tree<tkey, tvalue>::node*& subtree_root) const
{
    if (subtree_root != nullptr && subtree_root->right_subtree != nullptr && subtree_root->right_subtree->left_subtree != nullptr)
    {
        small_right_rotation(subtree_root->right_subtree);
        small_left_rotation(subtree_root);
    }
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::big_right_rotation(
    binary_search_tree<tkey, tvalue>::node*& subtree_root) const
{
    if (subtree_root != nullptr && subtree_root->left_subtree != nullptr && subtree_root->left_subtree->right_subtree != nullptr)
    {
        small_left_rotation(subtree_root->left_subtree);
        small_right_rotation(subtree_root);
    }
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::double_left_rotation(
    binary_search_tree<tkey, tvalue>::node*& subtree_root) const
{
    if (subtree_root != nullptr && subtree_root->right_subtree != nullptr &&
        subtree_root->right_subtree->right_subtree != nullptr)
    {
        small_left_rotation(subtree_root);
        small_left_rotation(subtree_root);
    }
}

template<typename tkey, typename tvalue>
void binary_search_tree<tkey, tvalue>::double_right_rotation(
    binary_search_tree<tkey, tvalue>::node*& subtree_root) const
{
    if (subtree_root != nullptr && subtree_root->left_subtree != nullptr && 
        subtree_root->left_subtree->left_subtree != nullptr)
    {
        small_right_rotation(subtree_root);
        small_right_rotation(subtree_root);
    }
}

// endregion subtree rotations implementation

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H