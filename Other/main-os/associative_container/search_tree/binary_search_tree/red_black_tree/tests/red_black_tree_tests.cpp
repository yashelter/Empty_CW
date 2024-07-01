#include "gtest/gtest.h"
#include <red_black_tree.h>
#include <associative_container.h>
#include <logger_builder.h>
#include <client_logger_builder.h>
#include <allocator_sorted_list.h>
#include <allocator_boundary_tags.h>
#include <allocator_global_heap.h>
#include <allocator_buddies_system.h>
#include <iostream>
#include <map>

logger *create_logger(
        std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
        bool use_console_stream = true,
        logger::severity console_stream_severity = logger::severity::debug)
{
    logger_builder *builder = new client_logger_builder();

    if (use_console_stream)
    {
        builder->add_console_stream(console_stream_severity);
    }

    for (auto &output_file_stream_setup: output_file_streams_setup)
    {
        builder->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
    }

    logger *built_logger = builder->build();

    delete builder;

    return built_logger;
}

class key_comparer final
{

public:

    int operator()(
            int first,
            int second) const
    {
        return first - second;
    }

    int operator()(
            std::string const &first,
            std::string const &second) const
    {
        if (first > second)
        {
            return 1;
        }
        else if (first < second)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

};

bool compare_results(
        std::vector<typename associative_container<int, std::string>::key_value_pair> &expected,
        std::vector<typename associative_container<int, std::string>::key_value_pair> &actual)
{
    key_comparer comparer;

    if (expected.size() != actual.size())
    {
        return false;
    }

    for (size_t i = 0; i < expected.size(); ++i)
    {
        if (comparer(expected[i].key, actual[i].key))
        {
            return false;
        }

        if (expected[i].value != actual[i].value)
        {
            return false;
        }
    }

    return true;
}

template<
        typename tkey,
        typename tvalue>
bool infix_iterator_test(
        red_black_tree<tkey, tvalue> const &tree,
        std::vector<typename red_black_tree<tkey, tvalue>::iterator_data> &expected_result)
{

    std::string line;
    auto end_infix = tree.cend_infix();
    auto it = tree.cbegin_infix();

    for (auto const &item: expected_result)
    {
        if ((*it)->depth != item.depth || (*it)->key != item.key || (*it)->value != item.value ||
            reinterpret_cast<typename red_black_tree<tkey, tvalue>::iterator_data const *>(*it)->_color != item._color)
        {
            return false;
        }
        ++it;
    }
    return true;
}

template<
        typename tkey,
        typename tvalue>
bool prefix_iterator_test(
        red_black_tree<tkey, tvalue> const &tree,
        std::vector<typename red_black_tree<tkey, tvalue>::iterator_data> &expected_result)
{
    std::string line;
    auto end_prefix = tree.cend_prefix();
    auto it = tree.cbegin_prefix();

    for (auto const &item: expected_result)
    {
        if ((*it)->depth != item.depth || (*it)->key != item.key || (*it)->value != item.value ||
            reinterpret_cast<typename red_black_tree<tkey, tvalue>::iterator_data const *>(*it)->_color != item._color)
        {
            return false;
        }
        ++it;
    }
    return true;
}

template<
        typename tkey,
        typename tvalue>
bool postfix_iterator_test(
        red_black_tree<tkey, tvalue> const &tree,
        std::vector<typename red_black_tree<tkey, tvalue>::iterator_data> &expected_result
)
{

    std::string line;
    auto end_postfix = tree.cend_postfix();
    auto it = tree.cbegin_postfix();

    for (
        auto const &item: expected_result)
    {
        if ((*it)->depth != item.depth || (*it)->key != item.key || (*it)->value != item.value ||
            reinterpret_cast<typename red_black_tree<tkey, tvalue>::iterator_data const *>(*it)->_color != item._color)
        {
            return false;
        }
        ++it;
    }
    return true;
}

TEST(redBlackTreePositiveTests, test1)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test1 started");

    //auto all = std::make_unique<allocator_sorted_list>(10000);
    //auto all = std::make_unique<allocator_buddies_system>(14);
    //auto alll = new allocator_global_heap(nullptr);
    auto all = std::make_unique<allocator_boundary_tags>(10000);

    auto rb = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    //auto rb = std::make_unique<binary_search_tree<int, std::string>>(key_comparer(), alll, nullptr);


    //search_tree<int, std::string> *rb = new red_black_tree<int, std::string>(key_comparer(), nullptr, logger);



    rb->insert(5, "a");
    rb->insert(2, "b");
    rb->insert(15, "c");
    rb->insert(3, "d");
    rb->insert(14, "e");
    rb->insert(1, "l");

    std::vector<typename red_black_tree<int, std::string>::iterator_data> expected_result =
            {
                    red_black_tree<int, std::string>::iterator_data(2, 1, "l", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(1, 2, "b", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(2, 3, "d", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(0, 5, "a", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(2, 14, "e", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(1, 15, "c", red_black_tree<int, std::string>::node_color::Black)
            };

    EXPECT_TRUE(infix_iterator_test(*reinterpret_cast<red_black_tree<int, std::string> *>(rb.get()), expected_result));

    logger->trace("redBlackTreePositiveTests.test1 finished");

    //delete rb;
    delete logger;
}

TEST(redBlackTreePositiveTests, test2)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test2 started");

    //auto all = std::make_unique<allocator_sorted_list>(10000);
    auto all = std::make_unique<allocator_buddies_system>(14);
    //auto alll = new allocator_global_heap(nullptr);
    //auto all = std::make_unique<allocator_boundary_tags>(10000);

    auto rb = std::make_unique<red_black_tree<int, int>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    //auto rb = std::make_unique<red_black_tree<int, int>>(key_comparer(), alll, nullptr);

    //search_tree<int, int> *rb = new red_black_tree<int, int>(key_comparer(), nullptr, logger);

    rb->insert(1, 5);
    rb->insert(2, 12);
    rb->insert(15, 1);
    rb->insert(3, 67);
    rb->insert(4, 45);

    std::vector<typename red_black_tree<int, int>::iterator_data> expected_result =
            {
                    red_black_tree<int, int>::iterator_data(0, 2, 12, red_black_tree<int, int>::node_color::Black),
                    red_black_tree<int, int>::iterator_data(1, 1, 5, red_black_tree<int, int>::node_color::Black),
                    red_black_tree<int, int>::iterator_data(1, 4, 45, red_black_tree<int, int>::node_color::Black),
                    red_black_tree<int, int>::iterator_data(2, 3, 67, red_black_tree<int, int>::node_color::Red),
                    red_black_tree<int, int>::iterator_data(2, 15, 1, red_black_tree<int, int>::node_color::Red)
            };

    EXPECT_TRUE(prefix_iterator_test(*reinterpret_cast<red_black_tree<int, int> *>(rb.get()), expected_result));

    logger->trace("redBlackTreePositiveTests.test2 finished");

    rb->insert(34, 45);
    rb->insert(123, 45);
    rb->insert(10, 45);
    rb->insert(56, 45);
    rb->insert(100, 45);
    rb->insert(7, 45);
    rb->insert(45, 45);
    rb->insert(78, 45);
    rb->insert(125, 45);
    rb->insert(98, 45);
    rb->insert(13, 45);
    rb->insert(36, 45);
    rb->insert(12, 45);
    rb->insert(55, 45);
    rb->insert(33, 45);
    rb->insert(44, 45);
    rb->insert(66, 45);
    rb->insert(77, 45);
    rb->insert(11, 45);
    rb->insert(22, 45);
    rb->insert(88, 45);
    rb->insert(99, 45);
    rb->insert(111, 45);
    rb->insert(101, 45);
    rb->insert(6, 45);
    rb->insert(67, 45);
    rb->insert(145, 45);
    rb->insert(24, 45);
    rb->insert(69, 45);
    rb->insert(19, 45);
    rb->insert(49, 45);

    //delete rb;
    delete logger;
}

TEST(redBlackTreePositiveTests, test3)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test3 started");

    auto all = std::make_unique<allocator_sorted_list>(10000);
    //auto all = std::make_unique<allocator_buddies_system>(14);
    //auto alll = new allocator_global_heap(nullptr);
    //auto all = std::make_unique<allocator_boundary_tags>(10000);

    auto rb = std::make_unique<red_black_tree<std::string, int>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    //auto rb = std::make_unique<binary_search_tree<int, std::string>>(key_comparer(), alll, nullptr);

    //search_tree<std::string, int> *rb = new red_black_tree<std::string, int>(key_comparer(), nullptr, logger);

    rb->insert("a", 1);
    rb->insert("b", 2);
    rb->insert("c", 15);
    rb->insert("d", 3);
    rb->insert("e", 4);

    std::vector<typename red_black_tree<std::string, int>::iterator_data> expected_result =
            {
                    red_black_tree<std::string, int>::iterator_data(1, "a", 1, red_black_tree<std::string, int>::node_color::Black),
                    red_black_tree<std::string, int>::iterator_data(2, "c", 15, red_black_tree<std::string, int>::node_color::Red),
                    red_black_tree<std::string, int>::iterator_data(2, "e", 4, red_black_tree<std::string, int>::node_color::Red),
                    red_black_tree<std::string, int>::iterator_data(1, "d", 3, red_black_tree<std::string, int>::node_color::Black),
                    red_black_tree<std::string, int>::iterator_data(0, "b", 2, red_black_tree<std::string, int>::node_color::Black)
            };

    EXPECT_TRUE(postfix_iterator_test(*reinterpret_cast<red_black_tree<std::string, int> *>(rb.get()), expected_result));

    logger->trace("redBlackTreePositiveTests.test3 finished");

    //delete rb;
    delete logger;
}

TEST(redBlackTreePositiveTests, test4)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test4 started");

    //auto all = std::make_unique<allocator_sorted_list>(10000);
    //auto all = std::make_unique<allocator_buddies_system>(14);
    auto alll = new allocator_global_heap(nullptr);
    //auto all = std::make_unique<allocator_boundary_tags>(10000);

    //auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), alll, nullptr);

    //search_tree<int, std::string> *rb1 = new red_black_tree<int, std::string>(key_comparer(), nullptr, logger);

    rb1->insert(6, "a");
    rb1->insert(8, "c");
    rb1->insert(15, "x");
    rb1->insert(4, "j");
    rb1->insert(1, "i");
    rb1->insert(5, "b");

    std::vector<typename red_black_tree<int, std::string>::iterator_data> expected_result =
            {
                    red_black_tree<int, std::string>::iterator_data(2, 1, "i", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 4, "j", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(3, 5, "b", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(2, 6, "a", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(0, 8, "c", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 15, "x", red_black_tree<int, std::string>::node_color::Black)
            };

    red_black_tree<int, std::string> rb2(
            std::move(*reinterpret_cast<red_black_tree<int, std::string> *>(rb1.get())));

    EXPECT_TRUE(infix_iterator_test(rb2, expected_result));

    logger->trace("redBlackTreePositiveTests.test4 finished");

    //delete rb1;
    delete logger;
}

TEST(redBlackTreePositiveTests, test5)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test5 started");

    //auto all = std::make_unique<allocator_sorted_list>(10000);
    //auto all = std::make_unique<allocator_buddies_system>(14);
    //auto alll = new allocator_global_heap(nullptr);
    auto all = std::make_unique<allocator_boundary_tags>(10000);

    auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    //auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), alll, nullptr);

    //search_tree<int, std::string> *rb1 = new red_black_tree<int, std::string>(key_comparer(), nullptr, logger);

    rb1->insert(6, "a");
    rb1->insert(8, "c");
    rb1->insert(15, "x");
    rb1->insert(4, "j");
    rb1->insert(1, "i");
    rb1->insert(5, "b");

    std::vector<typename red_black_tree<int, std::string>::iterator_data> expected_result =
            {
                    red_black_tree<int, std::string>::iterator_data(2, 1, "i", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 4, "j", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(3, 5, "b", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(2, 6, "a", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(0, 8, "c", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 15, "x", red_black_tree<int, std::string>::node_color::Black)
            };

    red_black_tree<int, std::string> rb2 = std::move(*reinterpret_cast<red_black_tree<int, std::string> *>(rb1.get()));

    EXPECT_TRUE(infix_iterator_test(rb2, expected_result));

    logger->trace("redBlackTreePositiveTests.test5 finished");

    //delete rb1;
    delete logger;
}

TEST(redBlackTreePositiveTests, test6)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test6 started");

    //auto all = std::make_unique<allocator_sorted_list>(10000);
    auto all = std::make_unique<allocator_buddies_system>(14);
    //auto alll = new allocator_global_heap(nullptr);
    //auto all = std::make_unique<allocator_boundary_tags>(10000);

    auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    //auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), alll, nullptr);

    //search_tree<int, std::string> *rb1 = new red_black_tree<int, std::string>(key_comparer(), nullptr, logger);

    rb1->insert(6, "a");
    rb1->insert(8, "c");
    rb1->insert(15, "x");
    rb1->insert(4, "j");
    rb1->insert(1, "i");
    rb1->insert(5, "b");

    rb1->dispose(5);

    std::vector<typename red_black_tree<int, std::string>::iterator_data> expected_result =
            {
                    red_black_tree<int, std::string>::iterator_data(2, 1, "i", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 4, "j", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(2, 6, "a", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(0, 8, "c", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 15, "x", red_black_tree<int, std::string>::node_color::Black)
            };

    EXPECT_TRUE(infix_iterator_test(*reinterpret_cast<red_black_tree<int, std::string> *>(rb1.get()), expected_result));

    logger->trace("redBlackTreePositiveTests.test6 finished");

    //delete rb1;
    delete logger;
}

TEST(redBlackTreePositiveTests, test7)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test7 started");

    auto all = std::make_unique<allocator_sorted_list>(10000);
    //auto all = std::make_unique<allocator_buddies_system>(14);
    //auto alll = new allocator_global_heap(nullptr);
    //auto all = std::make_unique<allocator_boundary_tags>(10000);

    auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    //auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), alll, nullptr);

    //search_tree<int, std::string> *rb1 = new red_black_tree<int, std::string>(key_comparer(), nullptr, logger);

    rb1->insert(6, "a");
    rb1->insert(8, "c");
    rb1->insert(15, "x");
    rb1->insert(4, "j");
    rb1->insert(3, "i");
    rb1->insert(2, "l");
    rb1->insert(5, "b");

    rb1->dispose(3);

    std::vector<typename red_black_tree<int, std::string>::iterator_data> expected_result =
            {
                    red_black_tree<int, std::string>::iterator_data(2, 2, "l", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 4, "j", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(3, 5, "b", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(2, 6, "a", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(0, 8, "c", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 15, "x", red_black_tree<int, std::string>::node_color::Black)
            };

    EXPECT_TRUE(infix_iterator_test(*reinterpret_cast<red_black_tree<int, std::string> *>(rb1.get()), expected_result));

    logger->trace("redBlackTreePositiveTests.test7 finished");

    //delete rb1;
    delete logger;

}

TEST(redBlackTreePositiveTests, test8)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test8 started");

    //auto all = std::make_unique<allocator_sorted_list>(10000);
    //auto all = std::make_unique<allocator_buddies_system>(14);
    auto alll = new allocator_global_heap(nullptr);
    //auto all = std::make_unique<allocator_boundary_tags>(10000);

    //auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), alll, nullptr);

    //search_tree<int, std::string> *rb1 = new red_black_tree<int, std::string>(key_comparer(), nullptr, logger);

    rb1->insert(6, "a");
    rb1->insert(8, "c");
    rb1->insert(15, "x");
    rb1->insert(11, "j");
    rb1->insert(19, "i");
    rb1->insert(12, "l");
    rb1->insert(17, "b");
    rb1->insert(18, "e");

    rb1->dispose(15);

    std::vector<typename red_black_tree<int, std::string>::iterator_data> expected_result =
            {
                    red_black_tree<int, std::string>::iterator_data(1, 6, "a", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(0, 8, "c", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(2, 11, "j", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 12, "l", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(3, 17, "b", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(2, 18, "e", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(3, 19, "i", red_black_tree<int, std::string>::node_color::Red)
            };

    EXPECT_TRUE(infix_iterator_test(*reinterpret_cast<red_black_tree<int, std::string> *>(rb1.get()), expected_result));

    logger->trace("redBlackTreePositiveTests.test8 finished");

    //delete rb1;
    delete logger;
}

TEST(redBlackTreePositiveTests, test9)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test9 started");

    //auto all = std::make_unique<allocator_sorted_list>(10000);
    //auto all = std::make_unique<allocator_buddies_system>(14);
    //auto alll = new allocator_global_heap(nullptr);
    auto all = std::make_unique<allocator_boundary_tags>(10000);

    auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    //auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), alll, nullptr);

    //search_tree<int, std::string> *rb1 = new red_black_tree<int, std::string>(key_comparer(), nullptr, logger);

    rb1->insert(6, "a");
    rb1->insert(8, "c");
    rb1->insert(15, "x");
    rb1->insert(11, "j");
    rb1->insert(19, "i");
    rb1->insert(12, "l");
    rb1->insert(17, "b");
    rb1->insert(18, "e");

    rb1->dispose(11);


    std::vector<typename red_black_tree<int, std::string>::iterator_data> expected_result =
            {
                    red_black_tree<int, std::string>::iterator_data(1, 6, "a", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(0, 8, "c", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(2, 12, "l", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(1, 15, "x", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(3, 17, "b", red_black_tree<int, std::string>::node_color::Red),
                    red_black_tree<int, std::string>::iterator_data(2, 18, "e", red_black_tree<int, std::string>::node_color::Black),
                    red_black_tree<int, std::string>::iterator_data(3, 19, "i", red_black_tree<int, std::string>::node_color::Red)
            };

    EXPECT_TRUE(infix_iterator_test(*reinterpret_cast<red_black_tree<int, std::string> *>(rb1.get()), expected_result));

    logger->trace("redBlackTreePositiveTests.test9 finished");

    //delete rb1;
    delete logger;

}

TEST(redBlackTreePositiveTests, test10)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   },
                                           });

    logger->trace("redBlackTreePositiveTests.test10 started");

    //auto all = std::make_unique<allocator_sorted_list>(10000);
    auto all = std::make_unique<allocator_buddies_system>(14);
    //auto alll = new allocator_global_heap(nullptr);
    //auto all = std::make_unique<allocator_boundary_tags>(10000);

    auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    //auto rb1 = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), alll, nullptr);

    //search_tree<int, std::string> *rb1 = new red_black_tree<int, std::string>(key_comparer(), nullptr, logger);

    rb1->insert(6, "l");
    rb1->insert(8, "c");
    rb1->insert(15, "l");
    rb1->insert(11, "o");
    rb1->insert(9, "h");
    rb1->insert(2, "e");
    rb1->insert(4, "b");
    rb1->insert(18, "e");

    std::vector<std::string> vector;

    vector.push_back(rb1->obtain(9));
    vector.push_back(rb1->obtain(2));
    vector.push_back(rb1->obtain(15));
    vector.push_back(rb1->obtain(6));
    vector.push_back(rb1->obtain(11));

    std::string actual_result;

    for (auto const &item: vector)
    {
        actual_result.append(item + " ");
    }

    EXPECT_EQ(actual_result, "h e l l o ");

    logger->trace("redBlackTreePositiveTests.test10 finished");

    //delete rb1;
    delete logger;

}

TEST(redBlackTreePositiveTests, test11)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   }
                                           });

    logger->trace("redBlackTreePositiveTests.test11 started");

    auto all = std::make_unique<allocator_sorted_list>(10000);
    //auto all = std::make_unique<allocator_buddies_system>(14);
    //auto alll = new allocator_global_heap(nullptr);
    //auto all = std::make_unique<allocator_boundary_tags>(10000);

    auto rb = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), all.get(), nullptr);
    //search_tree<int, std::string> *bst = new binary_search_tree<int, std::string>(key_comparer(), nullptr, nullptr);
    //auto rb = std::make_unique<red_black_tree<int, std::string>>(key_comparer(), alll, nullptr);

    //search_tree<int, std::string> *rb = new red_black_tree<int, std::string>(key_comparer(), nullptr, logger);

    rb->insert(6, "l");
    rb->insert(8, "c");
    rb->insert(15, "l");
    rb->insert(11, "o");
    rb->insert(9, "h");
    rb->insert(2, "e");
    rb->insert(4, "b");
    rb->insert(18, "e");

    std::vector<associative_container<int, std::string>::key_value_pair> actual_result = rb->obtain_between(2, 10, true, false);

    std::vector<associative_container<int, std::string>::key_value_pair> expected_result =
            {
                    { 2, "e" },
                    { 4, "b" },
                    { 6, "l" },
                    { 8, "c" },
                    { 9, "h" }
            };

    EXPECT_TRUE(compare_results(expected_result, actual_result));

    logger->trace("redBlackTreePositiveTests.test11 finished");

    //delete rb;
    delete logger;
}

TEST(own, test1)
{

    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                           {
                                                   {
                                                           "red_black_tree_tests_logs.txt",
                                                           logger::severity::trace
                                                   }
                                           });

    logger->trace("redBlackTreePositiveTests.test11 started");

    search_tree<int, int> *rb = new red_black_tree<int, int>(key_comparer(), nullptr, logger);

//    rb1->insert(6, "a");
//    rb1->insert(8, "c");
//    rb1->insert(15, "x");
//    rb1->insert(11, "j");
//    rb1->insert(19, "i");
//    rb1->insert(12, "l");
//    rb1->insert(17, "b");
//    rb1->insert(18, "e");
//
////    rb1->dispose(12);
////    rb1->dispose(11);
//
//    rb1->dispose(12);
//    rb1->dispose(19);
//    rb1->dispose(11);

//    rb->insert(1, 5);
//    rb->insert(2, 12);
//    rb->insert(15, 1);
//    rb->insert(3, 67);
//    rb->insert(4, 45);
//
//    rb->insert(34, 45);
//    rb->insert(123, 45);
//    rb->insert(10, 45);
//    rb->insert(56, 45);
//    rb->insert(100, 45);
//    rb->insert(7, 45);
//    rb->insert(45, 45);
//    rb->insert(78, 45);
//    rb->insert(125, 45);
//    rb->insert(98, 45);
//    rb->insert(13, 45);
//    rb->insert(36, 45);
//    rb->insert(12, 45);
//    rb->insert(55, 45);
//    rb->insert(33, 45);
//    rb->insert(44, 45);
//    rb->insert(66, 45);
//    rb->insert(77, 45);
//    rb->insert(11, 45);
//    rb->insert(22, 45);
//    rb->insert(88, 45);
//    rb->insert(99, 45);
//    rb->insert(111, 45);
//    rb->insert(101, 45);
//    rb->insert(6, 45);
//    rb->insert(67, 45);
//    rb->insert(145, 45);
//    rb->insert(24, 45);
//    rb->insert(69, 45);
//    rb->insert(19, 45);
//    rb->insert(49, 45);
//
//    rb->dispose(3);


//    rb->insert(135, 1);
//    rb->insert(106, 1);
//    rb->insert(245, 1);
//    rb->insert(54, 1);
//    rb->insert(188, 1);
//    rb->insert(282, 1);
//    rb->insert(222, 1);
//
//    rb->dispose(245);
//    rb->dispose(188);


    rb->insert(290, 1);
    rb->insert(241, 1);
    rb->insert(297, 1);
    rb->insert(236, 1);

    rb->dispose(236);
    rb->dispose(297);

}

TEST(own, test2)
{
    red_black_tree<int, int> tree{key_comparer()};
    std::map<int, int> map;

    size_t iterations = 100'000;

    srand(time(nullptr));

    for(size_t i = 0; i < iterations; ++i)
    {
        switch(rand() % 3)
        {
            case 0:
            case 1:
            {
                int tmp = rand();
                try
                {

                    if (map.find(tmp) == map.end())
                    {
                        map.insert(std::make_pair(tmp, 1));
                        tree.insert(tmp, 1);
                    }
                } catch (std::logic_error& er)
                {
                    std::cout << er.what() << std::endl;
                }
                break;
            }
            case 2:
            {
                if (!map.empty())
                {
                    auto it = map.begin();

                    std::advance(it, rand() % map.size());

                    tree.dispose(it->first);
                    map.erase(it);
                } else
                {
                    std::cout << "Empty" << std::endl;
                }
            }
                break;
        }
    }

    while(!map.empty())
    {
        auto it = map.begin();
        std::advance(it, rand() % map.size());

        tree.dispose(it->first);
        map.erase(it);
    }

}


int main(
        int argc,
        char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}