//
// Created by Des Caldnd on 6/2/2024.
//
//#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_VERSION 5
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <boost/thread/future.hpp>
#include <iostream>
#include "in_disk/tree/b_tree_disk.h"


struct int_wr
{
    int _data;

    void serialize(std::fstream& stream) const
    {
        stream.write(reinterpret_cast<const char*>(&_data), sizeof(int));
    }

    size_t serialize_size() const noexcept//как будто ненужёно
    {
        return sizeof(size_t);
    }

    static int_wr deserialize(std::fstream& stream)//потому что этот знает как себя читать
    {
        int_wr res;

        stream.read(reinterpret_cast<char*>(&res._data), sizeof(int));

        return res;
    }


};


class int_wrapper
{
//    size_t wrap_size;
public:
    int data;
    int_wrapper(int value) : data(value)
    {
        //wrap_size = sizeof(int);
    }
    int_wrapper() = default;

public:

    size_t serialize() const noexcept
    {
        return sizeof(int);
    }

};

struct comparer___
{
    bool operator()(int_wrapper l, int_wrapper r) const
    {
        return l.data < r.data;
    }
};

int main()
{
//    auto c = boost::async([](){ return 3; });
//
////    auto d = c.then([](auto x) {
////
////        return x.get() * 3;});
//
//    std::cout << c.get();

    B_tree_disk<int_wrapper, int_wrapper, comparer___, 2> btree("test.bin");//1

//    B_tree_disk<int_wrapper, int_wrapper, comparer___, 2>::btree_disk_node node = btree.disk_read(1);
//    std::cout<<node.position_in_disk << std::endl;
//    B_tree_disk<int_wrapper, int_wrapper, comparer___, 2>::btree_disk_node node2(false);//3
//    size_t pos = node2.position_in_disk;
//    std::cout<<pos<<std::endl;
//    btree.disk_write(node2);
//    B_tree_disk<int_wrapper, int_wrapper, comparer___, 2>::btree_disk_node node3 = btree.disk_read(pos);
//    std::cout<<node3.position_in_disk << std::endl;

    btree.insert({4,4});
    B_tree_disk<int_wrapper, int_wrapper, comparer___, 2>::btree_disk_node node = btree.disk_read(1);
    btree.insert({3,3});
    node = btree.disk_read(1);
    btree.insert({5,5});
    node = btree.disk_read(1);
    btree.insert({6,6});
    node = btree.disk_read(1);
}