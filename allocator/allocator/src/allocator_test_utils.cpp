#include "../include/allocator_test_utils.h"
#include <iostream>
#include <sstream>

bool allocator_test_utils::block_info::operator==(
    allocator_test_utils::block_info const &other) const noexcept
{
    return block_size == other.block_size
           && is_block_occupied == other.is_block_occupied;
}

bool allocator_test_utils::block_info::operator!=(
    allocator_test_utils::block_info const &other) const noexcept
{
    return !(*this == other);
}

std::string allocator_test_utils::print_blocks() const
{
    auto vec = get_blocks_info_inner();

    auto it = vec.begin();

    std::stringstream res;

    res << (it->is_block_occupied ? "occup" : "avail") << " " << std::to_string(it->block_size);

    ++it;

    for(auto end = vec.end(); it != end; ++it)
    {
        res << " | " << (it->is_block_occupied ? "occup" : "avail") << " " << std::to_string(it->block_size);
    }

    return res.str();
}
