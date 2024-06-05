//
// Created by Des Caldnd on 6/5/2024.
//
#include "b_tree.h"
#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <random>

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

int main(
        int argc,
        char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}