#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SCAPEGOAT_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SCAPEGOAT_TREE_H

#include <binary_search_tree.h>
#include <iterator>
#include <queue>

template<
    typename tkey,
    typename tvalue>
class scapegoat_tree final:
    public binary_search_tree<tkey, tvalue>
{

private:
    struct node final:
        binary_search_tree<tkey, tvalue>::node
    {
        size_t size;

        node(tkey const &key_, tvalue &&value_);
        node(tkey const &key_, const tvalue& value_);

        void recalculate_size() noexcept;
        bool is_disbalanced(double alpha) noexcept;
    };

public:

    explicit scapegoat_tree(
        std::function<int(tkey const &, tkey const &)> comparer,
        allocator *allocator = nullptr,
        logger *logger = nullptr,
        double alpha = 0.5);

public:

    ~scapegoat_tree() noexcept final =default;

    scapegoat_tree(scapegoat_tree<tkey, tvalue> const &other) =default;
    scapegoat_tree(scapegoat_tree<tkey, tvalue> &&other) noexcept =default;

    scapegoat_tree<tkey, tvalue> &operator=(scapegoat_tree<tkey, tvalue> const &other) =default;
    scapegoat_tree<tkey, tvalue> &operator=(scapegoat_tree<tkey, tvalue> &&other) noexcept =default;

public:

    void setup_alpha(double alpha);

private:

    double _alpha;

    void rebalance(typename binary_search_tree<tkey, tvalue>::node** top);
    void setup_alpha_inner(typename binary_search_tree<tkey, tvalue>::node** top);

    void insert_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node**>& node_path, const tkey& key, tvalue&& val) override;
    void insert_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node**>& node_path, const tkey& key, const tvalue& val) override;

    tvalue dispose_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node**>& node_path) override;

    template<typename tval_arg>
    void insert_inner_t(std::stack<typename binary_search_tree<tkey, tvalue>::node**>& node_path, const tkey& key, tval_arg&& val);

    static void remake_tree(typename binary_search_tree<tkey, tvalue>::node** where, std::vector<node*>::iterator begin, std::vector<node*>::iterator end);
};

template<typename tkey, typename tvalue>
tvalue scapegoat_tree<tkey, tvalue>::dispose_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path)
{
    tvalue res = (*node_path.top())->value;

    typename binary_search_tree<tkey, tvalue>::node* current_node = *node_path.top();

    if (current_node->right_subtree == nullptr && current_node->left_subtree == nullptr)
    {
        *node_path.top() = nullptr;
        node_path.pop();
    } else if (current_node->right_subtree == nullptr || current_node->left_subtree == nullptr)
    {
        typename binary_search_tree<tkey, tvalue>::node* node_of_interest = current_node->right_subtree != nullptr ? current_node->right_subtree : current_node->left_subtree;

        *node_path.top() = node_of_interest;
        ///node_path.pop();// strange

    } else
    {
        std::queue<typename binary_search_tree<tkey, tvalue>::node**> saved_path;
        typename binary_search_tree<tkey, tvalue>::node** target = &current_node->left_subtree;

        // searching max right from left
        while ((*target)->right_subtree != nullptr)
        {
            target = &((*target)->right_subtree);
            saved_path.push(target);
        }

        typename binary_search_tree<tkey, tvalue>::node* tmp = *target;

        *target = (*target)->left_subtree; // there exist only left subtree by definition
        *node_path.top() = tmp;

        tmp->left_subtree = current_node->left_subtree == tmp ?
                tmp->left_subtree :
                current_node->left_subtree;

        tmp->right_subtree = current_node->right_subtree;

        if (!saved_path.empty()) // we don't go to right subtree
        {
            node_path.push(&(*node_path.top())->left_subtree);
            saved_path.pop();
        }

        while (!saved_path.empty())
        {
            node_path.push(saved_path.front());
            saved_path.pop();
        }
    }

    allocator::destruct(current_node);
    allocator_guardant::deallocate_with_guard(current_node);

    // find scapegoat
    typename binary_search_tree<tkey, tvalue>::node** scapegoat = nullptr;

    while (!node_path.empty())
    {
        static_cast<node*>(*node_path.top())->recalculate_size();
        if (static_cast<node*>(*node_path.top())->is_disbalanced(_alpha))
        {
            scapegoat = node_path.top();
        }

        node_path.pop();
    }

    if (scapegoat != nullptr)
    {
        rebalance(scapegoat);
    }

    return res;
}

template<typename tkey, typename tvalue>
template<typename tval_arg>
void scapegoat_tree<tkey, tvalue>::insert_inner_t(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path,
                                                  const tkey &key, tval_arg &&val)
{
    *node_path.top() = static_cast<typename binary_search_tree<tkey, tvalue>::node*>(reinterpret_cast<node*>(allocator_guardant::allocate_with_guard(sizeof(node))));
    try
    {
        allocator::construct(static_cast<node*>(*node_path.top()), key, std::forward<tval_arg>(val));
    } catch(...)
    {
        allocator_guardant::deallocate_with_guard(static_cast<node*>(*node_path.top()));
        *node_path.top() = nullptr;
        throw;
    }

    typename binary_search_tree<tkey, tvalue>::node** scapegoat = nullptr;

    while (!node_path.empty())
    {
        static_cast<node*>(*node_path.top())->recalculate_size();
        if (static_cast<node*>(*node_path.top())->is_disbalanced(_alpha))
        {
            scapegoat = node_path.top();
        }

        node_path.pop();
    }

    if (scapegoat != nullptr)
    {
        rebalance(scapegoat);
    }
}

template<typename tkey, typename tvalue>
void scapegoat_tree<tkey, tvalue>::setup_alpha_inner(binary_search_tree<tkey, tvalue>::node **top)
{
    if (*top != nullptr)
    {
        bool is_balanced = static_cast<node*>(*top)->is_disbalanced(_alpha);

        if (is_balanced)
        {
            setup_alpha_inner(&(*top)->left_subtree);
            setup_alpha_inner(&(*top)->left_subtree);
        } else
        {
            rebalance(top);
        }
    }
}

template<typename tkey, typename tvalue>
void scapegoat_tree<tkey, tvalue>::remake_tree(binary_search_tree<tkey, tvalue>::node **where,
                                               std::vector<scapegoat_tree::node *>::iterator begin,
                                               std::vector<scapegoat_tree::node *>::iterator end)
{
    size_t length = std::distance(begin, end);

    if (length <= 0)
    {
        return;
    }
    auto middle = begin + (length / 2);

    *where = *middle;

    (*where)->left_subtree = nullptr;
    (*where)->right_subtree = nullptr;

    remake_tree(&(*where)->left_subtree, begin, middle);
    remake_tree(&(*where)->right_subtree, middle + 1, end);

    static_cast<node*>(*where)->recalculate_size();
}

template<typename tkey, typename tvalue>
void scapegoat_tree<tkey, tvalue>::rebalance(typename binary_search_tree<tkey, tvalue>::node **top)
{
    size_t size = static_cast<node*>(*top)->size;

    std::vector<node*> node_vec;
    node_vec.reserve(size);

    std::stack<typename binary_search_tree<tkey, tvalue>::node**> stack;
    stack.push(top);

    while ((*stack.top())->left_subtree != nullptr)
    {
        stack.push(&(*stack.top())->left_subtree);
    }

    typename binary_search_tree<tkey, tvalue>::infix_iterator it(stack, new typename binary_search_tree<tkey, tvalue>::iterator_data()), end;

    for(; it != end; ++it)
    {
        node_vec.push_back(static_cast<node*>(*binary_search_tree<tkey, tvalue>::get_last_node(it)));
    }

    auto beg = node_vec.begin(), e = node_vec.end();

    remake_tree(top, beg, e);
}

template<typename tkey, typename tvalue>
void scapegoat_tree<tkey, tvalue>::insert_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path,
                                                const tkey &key, tvalue &&val)
{
    insert_inner_t(node_path, key, std::move(val));
}

template<typename tkey, typename tvalue>
void scapegoat_tree<tkey, tvalue>::insert_inner(std::stack<typename binary_search_tree<tkey, tvalue>::node **> &node_path,
                                                const tkey &key, const tvalue &val)
{
    insert_inner_t(node_path, key, val);
}

template<typename tkey, typename tvalue>
bool scapegoat_tree<tkey, tvalue>::node::is_disbalanced(double alpha) noexcept
{
    size_t l = binary_search_tree<tkey, tvalue>::node::left_subtree == nullptr ? 0 : static_cast<node*>(binary_search_tree<tkey, tvalue>::node::left_subtree)->size;
    size_t r = binary_search_tree<tkey, tvalue>::node::right_subtree == nullptr ? 0 : static_cast<node*>(binary_search_tree<tkey, tvalue>::node::right_subtree)->size;

    return !((size * alpha >= l) && (size * alpha >= r));
}

template<typename tkey, typename tvalue>
void scapegoat_tree<tkey, tvalue>::node::recalculate_size() noexcept
{
    size_t l = binary_search_tree<tkey, tvalue>::node::left_subtree == nullptr ? 0 : static_cast<node*>(binary_search_tree<tkey, tvalue>::node::left_subtree)->size;
    size_t r = binary_search_tree<tkey, tvalue>::node::right_subtree == nullptr ? 0 : static_cast<node*>(binary_search_tree<tkey, tvalue>::node::right_subtree)->size;

    size = l + r + 1;
}

template<typename tkey, typename tvalue>
scapegoat_tree<tkey, tvalue>::node::node(const tkey &key_, const tvalue& value_): binary_search_tree<tkey, tvalue>::node(key_, value_), size(1) {}

template<typename tkey, typename tvalue>
scapegoat_tree<tkey, tvalue>::node::node(const tkey &key_, tvalue &&value_): binary_search_tree<tkey, tvalue>::node(key_, value_), size(1) {}

template<typename tkey, typename tvalue>
scapegoat_tree<tkey, tvalue>::scapegoat_tree(
    std::function<int(tkey const &, tkey const &)> comparer,
    allocator *allocator,
    logger *logger,
    double alpha) : binary_search_tree<tkey, tvalue>(comparer, allocator, logger)
{
        if (alpha < 0.5)
            alpha = 0.5;
        else if (alpha > 1)
            alpha = 1;

        _alpha = alpha;
}

template<typename tkey, typename tvalue>
void scapegoat_tree<tkey, tvalue>::setup_alpha(
    double alpha)
{
    if (alpha < 0.5)
        alpha = 0.5;
    else if (alpha > 1)
        alpha = 1;

    _alpha = alpha;

    setup_alpha_inner(&static_cast<node*>(binary_search_tree<tkey, tvalue>::_root));
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SCAPEGOAT_TREE_H