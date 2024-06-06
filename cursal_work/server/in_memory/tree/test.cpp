//
// Created by Des Caldnd on 6/5/2024.
//
#include "b_tree.h"
#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <random>
//#include <thread>

std::string generateRandomString(int length) {
    std::string str;
    const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, charset.size() - 1);
    for (int i = 0; i < length; ++i) {
        str += charset[distribution(generator)];
    }
    return str;
}

TEST(btr_test, 1test)
{
    B_tree<int, std::string> tree;

    std::unordered_map<int, std::string> map;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 1000000000);

    size_t iterations = 1'000'000;

    for(size_t i = 0; i < iterations; ++i)
    {
        auto num = distribution(generator);
        switch(distribution(generator) % 3)
        {
            case 0:
            case 1:

                if (!map.contains(num))
                {
//                    std::cout << "Insert: " << num << std::endl;
                    std::string str = generateRandomString(10);

                    map.insert(std::make_pair(num, str));
                    tree.insert(std::make_pair(num, str));
                }

                break;
            case 2:

                if (!map.empty())
                {
                    auto it = map.begin();

//                    std::cout << "Remove: " << it->first << std::endl;
                    tree.erase(it->first);
                    map.erase(it);
                }

                break;
        }
    }

    EXPECT_EQ(map.size(), tree.size());

    while(!map.empty())
    {
        auto it = map.begin();

        tree.erase(it->first);
        map.erase(it);
    }

    EXPECT_EQ(0, tree.size());
}

TEST(btr_test, 2test)
{
    B_tree<int, std::string> tree;

    tree.insert(std::make_pair(0, "a"));
    tree.insert(std::make_pair(1, "a"));
    tree.insert(std::make_pair(2, "a"));
}

using namespace std::chrono_literals;

TEST(btr_test, 3test)
{
    B_tree<int, std::string> tree;

    auto point = std::chrono::system_clock::now();
    std::this_thread::sleep_for(10ms);

    tree.insert({0, generateRandomString(10)});
    tree.insert({2, generateRandomString(10)});

    std::this_thread::sleep_for(10ms);

    std::cout << tree.at(0) << std::endl;

    tree.revert_to(point);

    auto it = tree.find(0);
    std::cout << (it == tree.end()) << std::endl;

    tree.redo_all();

    std::cout << tree.at(0) << std::endl;



}


TEST(btr_test, 4test)
{

    using tree_t = B_tree<int, std::string>;
    using coll_t = B_tree<std::string, tree_t>;

    coll_t tree;

    tree_t tr;
    tr.insert({1, "asd"});

    tree.insert({"abc", tr});

    coll_t tree2 = tree;

    std::cout << " ";
}

int main(
        int argc,
        char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}