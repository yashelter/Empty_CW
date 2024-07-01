#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SPLAY_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SPLAY_TREE_H

#include <binary_search_tree.h>

template<
    typename tkey,
    typename tvalue>
class splay_tree final:
    public binary_search_tree<tkey, tvalue>
{
public:
    explicit splay_tree(
        std::function<int(tkey const&, tkey const&)> comparer,
        allocator* allocator = nullptr,
        logger* logger = nullptr,
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy = binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::throw_an_exception,
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy = binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy::throw_an_exception);

public:

    ~splay_tree() noexcept final = default;

    splay_tree(splay_tree<tkey, tvalue> const& other) = default;
    splay_tree<tkey, tvalue>& operator=(splay_tree<tkey, tvalue> const& other) = default;

    splay_tree(splay_tree<tkey, tvalue>&& other) noexcept = default;
    splay_tree<tkey, tvalue>& operator=(splay_tree<tkey, tvalue>&& other) noexcept = default;

private:
    using node = binary_search_tree<tkey, tvalue>::node;

    void splay(std::stack<node**> path);

    void insert_inner(std::stack<node**>& node_path, const tkey& key, tvalue&& val) override;
    void insert_inner(std::stack<node**>& node_path, const tkey& key, const tvalue& val) override;

    tvalue& obtain_inner(std::stack<node**>& node_path) override;
    tvalue dispose_inner(std::stack<node**>& node_path) override;

    void actions_after_update_insert(std::stack<node**>& path) override;
};

template<typename tkey, typename tvalue>
void splay_tree<tkey, tvalue>::splay(std::stack<node**> path)
{
    while (path.size() >= 3)
    {
        node* current_node = *path.top();
        path.pop();

        bool second_trip_left, first_trip_left;

        second_trip_left = binary_search_tree<tkey, tvalue>::is_left_subtree(current_node, *path.top());

        current_node = *path.top();
        path.pop();

        first_trip_left = binary_search_tree<tkey, tvalue>::is_left_subtree(current_node, *path.top());

        if (first_trip_left)
        {
            if (second_trip_left)
            {
                binary_search_tree<tkey, tvalue>::double_right_rotation(*path.top());
            } else
            {
                binary_search_tree<tkey, tvalue>::big_right_rotation(*path.top());
            }
        } else if (!first_trip_left)
        {
            if (!second_trip_left)
            {
                binary_search_tree<tkey, tvalue>::double_left_rotation(*path.top());
            } else
            {
                binary_search_tree<tkey, tvalue>::big_left_rotation(*path.top());
            }
        }
    }

    if (path.size() >= 2)
    {
        node* current_node = *path.top();

        path.pop();

        bool trip_left;

        trip_left = binary_search_tree<tkey, tvalue>::is_left_subtree(current_node, *path.top());
        if (trip_left)
        {
            binary_search_tree<tkey, tvalue>::small_right_rotation(*path.top());
        } else
        {
            binary_search_tree<tkey, tvalue>::small_left_rotation(*path.top());
        }
    }
}

template<typename tkey, typename tvalue>
tvalue splay_tree<tkey, tvalue>::dispose_inner(std::stack<node**>& node_path)
{
    splay(node_path);

    std::stack<node**> path;

    auto res = binary_search_tree<tkey, tvalue>::_root->value;
    node* root = binary_search_tree<tkey, tvalue>::_root;
    node* right_at_root_st = binary_search_tree<tkey, tvalue>::_root->right_subtree;

    // took max right in left
    binary_search_tree<tkey, tvalue>::_root = binary_search_tree<tkey, tvalue>::_root->left_subtree;

    path.push(&(binary_search_tree<tkey, tvalue>::_root));

    while ((*path.top())->right_subtree != nullptr)
    {
        path.push(&((*path.top())->right_subtree));
    }

    splay(path);

    binary_search_tree<tkey, tvalue>::_root->right_subtree = right_at_root_st;

    allocator::destruct(root);
    allocator_guardant::deallocate_with_guard(root);

    return res;
}

template<typename tkey, typename tvalue>
tvalue& splay_tree<tkey, tvalue>::obtain_inner(std::stack<node**>& node_path)
{
    splay(node_path);

    return binary_search_tree<tkey, tvalue>::_root->value;
}

template<typename tkey, typename tvalue>
void splay_tree<tkey, tvalue>::actions_after_update_insert(std::stack<node**>& path)
{
    splay(path);
}

template<typename tkey, typename tvalue>
void splay_tree<tkey, tvalue>::insert_inner(std::stack<node**>& node_path, const tkey& key, tvalue&& val)
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

    splay(node_path);
}

template<typename tkey, typename tvalue>
void splay_tree<tkey, tvalue>::insert_inner(std::stack<node**>& node_path, const tkey& key, const tvalue& val)
{
    *node_path.top() = reinterpret_cast<node*>(allocator_guardant::allocate_with_guard(sizeof(node)));
    try
    {
        allocator::construct(*node_path.top(), key, val);
    } catch (...)
    {
        allocator_guardant::deallocate_with_guard(*node_path.top());
        *node_path.top() = nullptr;
        throw;
    }

    splay(node_path);
}

template<typename tkey, typename tvalue>
splay_tree<tkey, tvalue>::splay_tree(std::function<int(const tkey&, const tkey&)> comparer, allocator* allocator,
    logger* logger,
    binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy,
    binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy)
    :binary_search_tree<tkey, tvalue>(comparer, allocator, logger, insertion_strategy, disposal_strategy) {}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SPLAY_TREE_H