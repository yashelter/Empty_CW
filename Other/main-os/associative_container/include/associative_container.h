#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ASSOCIATIVE_CONTAINER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ASSOCIATIVE_CONTAINER_H

#include <iostream>
#include <vector>
#include <operation_not_supported.h>

template<
    typename tkey,
    typename tvalue>
class associative_container
{

public:

    struct key_value_pair final
    {

    public:

        tkey key;
        tvalue value;

        bool operator==(const key_value_pair& other) const noexcept;
    };

    struct key_value_ptr_pair
    {

    public:

        tkey key;
        tvalue* value_ptr;

    };

public:

    // TODO
    struct associative_container_iterator
    {

    public:

    };

public:

    virtual ~associative_container() noexcept = default;

public:

    virtual void insert(
        tkey const& key,
        tvalue const& value) = 0;

    virtual void insert(
        tkey const& key,
        tvalue&& value) = 0;

    virtual tvalue const& obtain(
        tkey const& key) = 0;

    virtual tvalue dispose(
        tkey const& key) = 0;

};

template<typename tkey, typename tvalue>
bool associative_container<tkey, tvalue>::key_value_pair::operator==(
    const associative_container::key_value_pair& other) const noexcept
{
    return key == other.key && value == other.value;
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ASSOCIATIVE_CONTAINER_H