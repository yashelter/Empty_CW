#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_RED_BLACK_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_RED_BLACK_TREE_H

#include <binary_search_tree.h>
#include <queue>

template<
        typename tkey,
        typename tvalue>
class red_black_tree final :
        public binary_search_tree<tkey, tvalue>
{

public:

    enum class node_color : unsigned char
    {
        Red,
        Black
    };

private:

    struct node final :
            binary_search_tree<tkey, tvalue>::node
    {
        node_color color;

        node(tkey const &key_, tvalue &&value_);

        node(tkey const &key_, const tvalue &value_);

        ~node() noexcept override = default;
    };

public:

    struct iterator_data final :
            public binary_search_tree<tkey, tvalue>::iterator_data
    {
    private:
        void update(const typename binary_search_tree<tkey, tvalue>::node *n, unsigned int _depth) override;

        typename binary_search_tree<tkey, tvalue>::iterator_data *clone() override;

    public:

        node_color _color;

    public:

        explicit iterator_data(
                unsigned int depth = 0,
                tkey const &key = tkey(),
                tvalue const &value = tvalue(),
                node_color _color = node_color::Red);

    };

public:

    explicit red_black_tree(
            std::function<int(tkey const &, tkey const &)> comparer = std::less<tkey>(),
            allocator *allocator = nullptr,
            logger *logger = nullptr,
            typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy = binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::throw_an_exception,
            typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy = binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy::throw_an_exception);


    // region iterator definition

    class prefix_iterator : public binary_search_tree<tkey, tvalue>::prefix_iterator
    {
    public:

        explicit prefix_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::prefix_iterator(path, data)
        {}

    };

    class prefix_const_iterator : public binary_search_tree<tkey, tvalue>::prefix_const_iterator
    {
    public:

        explicit prefix_const_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::prefix_const_iterator(path, data)
        {}

    };

    class prefix_reverse_iterator : public binary_search_tree<tkey, tvalue>::prefix_reverse_iterator
    {
    public:

        explicit prefix_reverse_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::prefix_reverse_iterator(path, data)
        {}

    };

    class prefix_const_reverse_iterator : public binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator
    {
    public:

        explicit prefix_const_reverse_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator(path,
                                                                                                                 data)
        {}

    };

    class infix_iterator : public binary_search_tree<tkey, tvalue>::infix_iterator
    {
    public:

        explicit infix_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::infix_iterator(path, data)
        {}

    };

    class infix_const_iterator : public binary_search_tree<tkey, tvalue>::infix_const_iterator
    {
    public:

        explicit infix_const_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::infix_const_iterator(path, data)
        {}

    };

    class infix_reverse_iterator : public binary_search_tree<tkey, tvalue>::infix_reverse_iterator
    {
    public:

        explicit infix_reverse_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::infix_reverse_iterator(path, data)
        {}

    };

    class infix_const_reverse_iterator : public binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator
    {
    public:

        explicit infix_const_reverse_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator(path,
                                                                                                                data)
        {}

    };

    class postfix_iterator : public binary_search_tree<tkey, tvalue>::postfix_iterator
    {
    public:

        explicit postfix_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::postfix_iterator(path, data)
        {}

    };

    class postfix_const_iterator : public binary_search_tree<tkey, tvalue>::postfix_const_iterator
    {
    public:

        explicit postfix_const_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::postfix_const_iterator(path, data)
        {}

    };

    class postfix_reverse_iterator : public binary_search_tree<tkey, tvalue>::postfix_reverse_iterator
    {
    public:

        explicit postfix_reverse_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::postfix_reverse_iterator(path, data)
        {}

    };

    class postfix_const_reverse_iterator : public binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator
    {
    public:

        explicit postfix_const_reverse_iterator(
                const std::stack<typename binary_search_tree<tkey, tvalue>::node **> &path = std::stack<typename binary_search_tree<tkey, tvalue>::node **>(),
                iterator_data *data = nullptr) : binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator(path,
                                                                                                                  data)
        {}

    };
    // endregion iterator definition

    // region iterator requests declaration

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

    // endregion iterator requests declaration

public:

    ~red_black_tree() noexcept final = default;

    red_black_tree(red_black_tree<tkey, tvalue> const &other) = default;
    red_black_tree<tkey, tvalue> &operator=(red_black_tree<tkey, tvalue> const &other) = default;
    red_black_tree(red_black_tree<tkey, tvalue> &&other) noexcept = default;
    red_black_tree<tkey, tvalue> &operator=(red_black_tree<tkey, tvalue> &&other) noexcept = default;

private:

    void copy_subtree(typename binary_search_tree<tkey, tvalue>::node **target,
                      typename binary_search_tree<tkey, tvalue>::node *src) override;

    tvalue dispose_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path) override;

    void insert_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path, const tkey &key,
                      tvalue &&val) override;

    void insert_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path, const tkey &key,
                      const tvalue &val) override;

    template<typename tval_arg>
    void insert_inner_t(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path, const tkey &key,
                        tval_arg &&val);

    void balance_black_leaf(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path);
};

template<typename tkey, typename tvalue>
void red_black_tree<tkey, tvalue>::balance_black_leaf(
        std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path)
{
    // https://habr.com/ru/articles/573502/
    if (node_path.size() == 1 && *node_path.top() != nullptr)
    {
        static_cast<node *>(*node_path.top())->color = node_color::Black;
    } else if (node_path.size() > 1)
    {
        typename binary_search_tree<tkey, tvalue>::node *current_node = *node_path.top();
        node_path.pop();

        typename binary_search_tree<tkey, tvalue>::node *parent_node = *node_path.top();

        bool from_left = binary_search_tree<tkey, tvalue>::is_left_subtree(current_node, parent_node);

        typename binary_search_tree<tkey, tvalue>::node *brother_node = from_left ? parent_node->right_subtree
                                                                                  : parent_node->left_subtree;

        if (static_cast<node *>(brother_node)->color == node_color::Red)
        {
            if (from_left)
            {
                binary_search_tree<tkey, tvalue>::small_left_rotation(*node_path.top());
                node_path.push(&((*node_path.top())->left_subtree));
                node_path.push(&((*node_path.top())->left_subtree));
            } else
            {
                binary_search_tree<tkey, tvalue>::small_right_rotation(*node_path.top());
                node_path.push(&((*node_path.top())->right_subtree));
                node_path.push(&((*node_path.top())->right_subtree));
            }
            static_cast<node *>(parent_node)->color = node_color::Red;
            static_cast<node *>(brother_node)->color = node_color::Black;

            balance_black_leaf(node_path);
        } else
        {
            typename binary_search_tree<tkey, tvalue>::node *far_nephew = from_left ? brother_node->right_subtree
                                                                                    : brother_node->left_subtree;
            typename binary_search_tree<tkey, tvalue>::node *near_nephew = from_left ? brother_node->left_subtree
                                                                                     : brother_node->right_subtree;

            if (far_nephew != nullptr && static_cast<node *>(far_nephew)->color == node_color::Red)
            {
                if (from_left)
                {
                    binary_search_tree<tkey, tvalue>::small_left_rotation(*node_path.top());
                } else
                {
                    binary_search_tree<tkey, tvalue>::small_right_rotation(*node_path.top());
                }

                static_cast<node *>(brother_node)->color = static_cast<node *>(parent_node)->color;
                static_cast<node *>(parent_node)->color = node_color::Black;
                static_cast<node *>(far_nephew)->color = node_color::Black;
            } else if (near_nephew != nullptr && static_cast<node *>(near_nephew)->color == node_color::Red)
            {
                if (from_left)
                {
                    binary_search_tree<tkey, tvalue>::big_left_rotation(*node_path.top());
                } else
                {
                    binary_search_tree<tkey, tvalue>::big_right_rotation(*node_path.top());
                }

                static_cast<node *>(near_nephew)->color = static_cast<node *>(parent_node)->color;
                static_cast<node *>(parent_node)->color = node_color::Black;
            } else
            {
                static_cast<node *>(brother_node)->color = node_color::Red;

                if (static_cast<node *>(parent_node)->color == node_color::Red)
                {
                    static_cast<node *>(parent_node)->color = node_color::Black;
                } else
                {
                    balance_black_leaf(node_path);
                }
            }
        }
    }
}

template<typename tkey, typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data *red_black_tree<tkey, tvalue>::iterator_data::clone()
{
    return new iterator_data(*this);
}

template<typename tkey, typename tvalue>
void red_black_tree<tkey, tvalue>::iterator_data::update(const typename binary_search_tree<tkey, tvalue>::node *n,
                                                         unsigned int _depth)
{
    binary_search_tree<tkey, tvalue>::iterator_data::key = n->key;
    binary_search_tree<tkey, tvalue>::iterator_data::value = n->value;
    binary_search_tree<tkey, tvalue>::iterator_data::depth = _depth;
    _color = static_cast<const node *>(n)->color;
}

template<typename tkey, typename tvalue>
template<typename tval_arg>
void
red_black_tree<tkey, tvalue>::insert_inner_t(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path,
                                             const tkey &key, tval_arg &&val)
{
    *node_path.top() = static_cast<typename binary_search_tree<tkey, tvalue>::node *>(reinterpret_cast<node *>(allocator_guardant::allocate_with_guard(
            sizeof(node))));
    try
    {
        allocator::construct(static_cast<node *>(*node_path.top()), key, std::forward<tval_arg>(val));
    } catch (...)
    {
        allocator_guardant::deallocate_with_guard(static_cast<node *>(*node_path.top()));
        *node_path.top() = nullptr;
        throw;
    }

    bool need_continue = true;

    while (need_continue)
    {
        need_continue = false;

        if (node_path.size() == 1)
        {
            static_cast<node *>(*node_path.top())->color = node_color::Black;
        } else if (node_path.size() >= 3 && static_cast<node *>(*node_path.top())->color == node_color::Red)
        {
            typename binary_search_tree<tkey, tvalue>::node *child_node = *node_path.top();
            node_path.pop();

            if (static_cast<node *>(*node_path.top())->color == node_color::Red) // double red
            {
                typename binary_search_tree<tkey, tvalue>::node *parent_node = *node_path.top();
                bool second_turn_left = binary_search_tree<tkey, tvalue>::is_left_subtree(child_node, parent_node);

                node_path.pop();

                typename binary_search_tree<tkey, tvalue>::node *grandparent_node = *node_path.top();

                bool first_turn_left = binary_search_tree<tkey, tvalue>::is_left_subtree(parent_node, grandparent_node);

                typename binary_search_tree<tkey, tvalue>::node *uncle_node = first_turn_left
                                                                              ? (*node_path.top())->right_subtree
                                                                              : (*node_path.top())->left_subtree;

                if (uncle_node == nullptr || static_cast<node *>(uncle_node)->color == node_color::Black)
                {
                    if (first_turn_left)
                    {
                        if (second_turn_left)
                        {
                            binary_search_tree<tkey, tvalue>::small_right_rotation(*node_path.top());
                            static_cast<node *>(child_node)->color = node_color::Red;
                            static_cast<node *>(parent_node)->color = node_color::Black;
                            static_cast<node *>(grandparent_node)->color = node_color::Red;
                        } else
                        {
                            binary_search_tree<tkey, tvalue>::big_right_rotation(*node_path.top());
                            static_cast<node *>(child_node)->color = node_color::Black;
                            static_cast<node *>(parent_node)->color = node_color::Red;
                            static_cast<node *>(grandparent_node)->color = node_color::Red;
                        }
                    } else
                    {
                        if (!second_turn_left)
                        {
                            binary_search_tree<tkey, tvalue>::small_left_rotation(*node_path.top());
                            static_cast<node *>(child_node)->color = node_color::Red;
                            static_cast<node *>(parent_node)->color = node_color::Black;
                            static_cast<node *>(grandparent_node)->color = node_color::Red;
                        } else
                        {
                            binary_search_tree<tkey, tvalue>::big_left_rotation(*node_path.top());
                            static_cast<node *>(child_node)->color = node_color::Black;
                            static_cast<node *>(parent_node)->color = node_color::Red;
                            static_cast<node *>(grandparent_node)->color = node_color::Red;
                        }
                    }
                }
                else
                {
                    need_continue = true;

                    static_cast<node *>(uncle_node)->color = node_color::Black;
                    static_cast<node *>(parent_node)->color = node_color::Black;
                    static_cast<node *>(grandparent_node)->color = node_color::Red;
                }

            }
        }
    }
}

template<typename tkey, typename tvalue>
tvalue red_black_tree<tkey, tvalue>::dispose_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path)
{
    tvalue res = (*node_path.top())->value;

    typename binary_search_tree<tkey, tvalue>::node *current_node = *node_path.top();

    bool bad_way = false;

    if (current_node->right_subtree == nullptr && current_node->left_subtree == nullptr)
    {
        bad_way = static_cast<node *>(*node_path.top())->color == node_color::Black; // black leaf ^_^
        *node_path.top() = nullptr;
    } else if (current_node->right_subtree == nullptr || current_node->left_subtree == nullptr)
    {
        // interesting case
        typename binary_search_tree<tkey, tvalue>::node *current_current =
                current_node->right_subtree != nullptr ? current_node->right_subtree : current_node->left_subtree;

        node_color color = static_cast<node *>(*node_path.top())->color;

        if (color == node_color::Black)
        {
            static_cast<node *>(current_current)->color = node_color::Black;
        }

        *node_path.top() = current_current;
        node_path.pop();
    } else
    {
        std::queue<typename binary_search_tree<tkey, tvalue>::node **> saved_path;
        typename binary_search_tree<tkey, tvalue>::node **current_current = &current_node->left_subtree;

        while ((*current_current)->right_subtree != nullptr)
        {
            current_current = &((*current_current)->right_subtree);
            saved_path.push(current_current);
        }

        typename binary_search_tree<tkey, tvalue>::node *tmp = *current_current;

        bad_way = static_cast<node *>(tmp)->color == node_color::Black && tmp->left_subtree == nullptr &&
                         tmp->right_subtree == nullptr;

        if (static_cast<node *>(tmp)->color == node_color::Black && tmp->left_subtree != nullptr)
        {
            typename binary_search_tree<tkey, tvalue>::node *bottom_node = tmp->left_subtree;
            static_cast<node *>(bottom_node)->color = node_color::Black;
        }

        std::swap(static_cast<node *>(*current_current)->color, static_cast<node *>(current_node)->color);
        *current_current = (*current_current)->left_subtree;
        *node_path.top() = tmp;

        tmp->left_subtree = current_node->left_subtree == tmp ? tmp->left_subtree : current_node->left_subtree;
        tmp->right_subtree = current_node->right_subtree;

        node_path.push(&(*node_path.top())->left_subtree);

        while (!saved_path.empty())
        {
            node_path.push(saved_path.front());
            saved_path.pop();
        }
    }

    allocator::destruct(current_node);
    allocator_guardant::deallocate_with_guard(current_node);

    if (bad_way)
    {
        balance_black_leaf(node_path);
    }

    return res;
}

template<typename tkey, typename tvalue>
void
red_black_tree<tkey, tvalue>::insert_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path,
                                           const tkey &key, tvalue &&val)
{
    insert_inner_t(node_path, key, std::move(val));
}

template<typename tkey, typename tvalue>
void
red_black_tree<tkey, tvalue>::insert_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path,
                                           const tkey &key, const tvalue &val)
{
    insert_inner_t(node_path, key, val);
}

template<typename tkey, typename tvalue>
void red_black_tree<tkey, tvalue>::copy_subtree(typename binary_search_tree<tkey, tvalue>::node **target,
                                                typename binary_search_tree<tkey, tvalue>::node *src)
{
    if (src == nullptr)
    {
        *target = nullptr;
        return;
    }

    *target = static_cast<typename binary_search_tree<tkey, tvalue>::node *>(reinterpret_cast<node *>(allocator_guardant::allocate_with_guard(
            sizeof(node))));
    try
    {
        allocator::construct(static_cast<node *>(*target), *static_cast<node *>(src));
    } catch (...)
    {
        allocator_guardant::deallocate_with_guard(static_cast<node *>(*target));
        *target = nullptr;
        throw;
    }

    (*target)->right_subtree = nullptr;
    (*target)->left_subtree = nullptr;

    copy_subtree(&((*target)->left_subtree), src->left_subtree);
    copy_subtree(&((*target)->right_subtree), src->right_subtree);
}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::node::node(const tkey &key_, tvalue &&value_) : binary_search_tree<tkey, tvalue>::node(
        key_, std::move(value_)), color(node_color::Red)
{}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::node::node(const tkey &key_, const tvalue &value_)
        : binary_search_tree<tkey, tvalue>::node(key_, value_), color(node_color::Red)
{}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::iterator_data::iterator_data(
        unsigned int depth,
        tkey const &key,
        tvalue const &value,
        typename red_black_tree<tkey, tvalue>::node_color _color) :
        binary_search_tree<tkey, tvalue>::iterator_data(depth, key, value), _color(_color)
{}

template<typename tkey, typename tvalue>
red_black_tree<tkey, tvalue>::red_black_tree(
        std::function<int(tkey const &, tkey const &)> comparer,
        allocator *allocator,
        logger *logger,
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy,
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy)
        : binary_search_tree<tkey, tvalue>(comparer, allocator, logger, insertion_strategy, disposal_strategy)
{}

// region iterator requests definition

template<typename tkey, typename tvalue>
typename red_black_tree<tkey, tvalue>::prefix_iterator red_black_tree<tkey, tvalue>::begin_prefix() noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    if (binary_search_tree<tkey, tvalue>::_root != nullptr)
    {
        stack.push(&(binary_search_tree<tkey, tvalue>::_root));
    }

    return prefix_iterator(stack, new iterator_data);
}

template<typename tkey, typename tvalue>
typename red_black_tree<tkey, tvalue>::prefix_iterator red_black_tree<tkey, tvalue>::end_prefix() noexcept
{
    return prefix_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::prefix_const_iterator
red_black_tree<tkey, tvalue>::cbegin_prefix() const noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    if (binary_search_tree<tkey, tvalue>::_root != nullptr)
    {
        stack.push(&(binary_search_tree<tkey, tvalue>::_root));
    }

    return prefix_const_iterator(stack, new iterator_data);
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::prefix_const_iterator red_black_tree<tkey, tvalue>::cend_prefix() const noexcept
{
    return prefix_const_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::prefix_reverse_iterator red_black_tree<tkey, tvalue>::rbegin_prefix() noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    if (binary_search_tree<tkey, tvalue>::_root != nullptr)
    {
        stack.push(&(binary_search_tree<tkey, tvalue>::_root));
    }

    return prefix_reverse_iterator(stack, new iterator_data);
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::prefix_reverse_iterator red_black_tree<tkey, tvalue>::rend_prefix() noexcept
{
    return prefix_reverse_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::prefix_const_reverse_iterator
red_black_tree<tkey, tvalue>::crbegin_prefix() const noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    if (binary_search_tree<tkey, tvalue>::_root != nullptr)
    {
        stack.push(&(binary_search_tree<tkey, tvalue>::_root));
    }

    return prefix_const_reverse_iterator(stack, new iterator_data);
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::prefix_const_reverse_iterator
red_black_tree<tkey, tvalue>::crend_prefix() const noexcept
{
    return prefix_const_reverse_iterator();
}

template<typename tkey, typename tvalue>
typename red_black_tree<tkey, tvalue>::infix_iterator red_black_tree<tkey, tvalue>::begin_infix() noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    typename binary_search_tree<tkey, tvalue>::node **current_node = &(binary_search_tree<tkey, tvalue>::_root);
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = &((*current_node)->left_subtree);
    }
    return infix_iterator(stack, new iterator_data);
}

template<typename tkey, typename tvalue>
typename red_black_tree<tkey, tvalue>::infix_iterator red_black_tree<tkey, tvalue>::end_infix() noexcept
{
    return infix_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::infix_const_iterator red_black_tree<tkey, tvalue>::cbegin_infix() const noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    typename binary_search_tree<tkey, tvalue>::node **current_node = &(binary_search_tree<tkey, tvalue>::_root);
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
typename red_black_tree<tkey, tvalue>::infix_const_iterator red_black_tree<tkey, tvalue>::cend_infix() const noexcept
{
    return infix_const_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::infix_reverse_iterator red_black_tree<tkey, tvalue>::rbegin_infix() noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    typename binary_search_tree<tkey, tvalue>::node **current_node = &(binary_search_tree<tkey, tvalue>::_root);
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
typename red_black_tree<tkey, tvalue>::infix_reverse_iterator red_black_tree<tkey, tvalue>::rend_infix() noexcept
{
    return infix_reverse_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue>::crbegin_infix() const noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    typename binary_search_tree<tkey, tvalue>::node **current_node = &(binary_search_tree<tkey, tvalue>::_root);
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
typename red_black_tree<tkey, tvalue>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue>::crend_infix() const noexcept
{
    return infix_const_reverse_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::postfix_iterator red_black_tree<tkey, tvalue>::begin_postfix() noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    typename binary_search_tree<tkey, tvalue>::node **current_node = &(binary_search_tree<tkey, tvalue>::_root);
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = (*current_node)->left_subtree != nullptr ? &((*current_node)->left_subtree)
                                                                : &((*current_node)->right_subtree);
    }
    return postfix_iterator(stack, new iterator_data);
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::postfix_iterator red_black_tree<tkey, tvalue>::end_postfix() noexcept
{
    return postfix_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::postfix_const_iterator
red_black_tree<tkey, tvalue>::cbegin_postfix() const noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    typename binary_search_tree<tkey, tvalue>::node **current_node = &(binary_search_tree<tkey, tvalue>::_root);
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = (*current_node)->left_subtree != nullptr ? &((*current_node)->left_subtree)
                                                                : &((*current_node)->right_subtree);
    }
    return postfix_const_iterator(stack, new iterator_data);
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::postfix_const_iterator
red_black_tree<tkey, tvalue>::cend_postfix() const noexcept
{
    return postfix_const_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::postfix_reverse_iterator red_black_tree<tkey, tvalue>::rbegin_postfix() noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    typename binary_search_tree<tkey, tvalue>::node **current_node = &(binary_search_tree<tkey, tvalue>::_root);
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = (*current_node)->right_subtree != nullptr ? &((*current_node)->right_subtree)
                                                                 : &((*current_node)->left_subtree);
    }
    return postfix_reverse_iterator(stack, new iterator_data);
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::postfix_reverse_iterator red_black_tree<tkey, tvalue>::rend_postfix() noexcept
{
    return postfix_reverse_iterator();
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::postfix_const_reverse_iterator
red_black_tree<tkey, tvalue>::crbegin_postfix() const noexcept
{
    std::stack<typename binary_search_tree<tkey, tvalue>::node **> stack;
    typename binary_search_tree<tkey, tvalue>::node **current_node = &(binary_search_tree<tkey, tvalue>::_root);
    while (*current_node != nullptr)
    {
        stack.push(current_node);
        current_node = (*current_node)->right_subtree != nullptr ? &((*current_node)->right_subtree)
                                                                 : &((*current_node)->left_subtree);
    }
    return postfix_const_reverse_iterator(stack, new iterator_data);
}

template<
        typename tkey,
        typename tvalue>
typename red_black_tree<tkey, tvalue>::postfix_const_reverse_iterator
red_black_tree<tkey, tvalue>::crend_postfix() const noexcept
{
    return postfix_const_reverse_iterator();
}

// endregion iterator requests definition


#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_RED_BLACK_TREE_H