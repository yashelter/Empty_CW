#include <gtest/gtest.h>
#include <logger.h>
#include <logger_builder.h>
#include <client_logger_builder.h>
#include <list>

#include "../include/allocator_sorted_list.h"

logger* create_logger(
    std::vector<std::pair<std::string, logger::severity>> const& output_file_streams_setup,
    bool use_console_stream = true,
    logger::severity console_stream_severity = logger::severity::debug)
{
    client_logger_builder* logger_builder_instance = new client_logger_builder;

    logger_builder_instance->set_format("[%s] %m\n");

    if (use_console_stream)
    {
        //logger_builder_instance->add_console_stream(logger::severity::debug);
        //logger_builder_instance->add_console_stream(logger::severity::trace);
        logger_builder_instance->add_console_stream(logger::severity::error);
        //logger_builder_instance->add_console_stream(logger::severity::warning);
        //logger_builder_instance->add_console_stream(logger::severity::information);
        //logger_builder_instance->add_console_stream(logger::severity::critical);
    }


    for (auto& output_file_stream_setup : output_file_streams_setup)
    {
        logger_builder_instance->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
    }

    logger* logger_instance = logger_builder_instance->build();

    delete logger_builder_instance;

    return logger_instance;
}

TEST(allocatorSortedListPositiveTests, test1)
{
    logger* logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "a.txt",
                logger::severity::information
        }
    });

    allocator* alloc = new allocator_sorted_list(3000, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);

    auto first_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 250));

    auto second_block = reinterpret_cast<char*>(alloc->allocate(sizeof(int), 250));
    alloc->deallocate(first_block);

    first_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 245));

    auto* the_same_subject = dynamic_cast<allocator_with_fit_mode*>(alloc);
    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_best_fit);

    auto third_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 1));

    alloc->deallocate(second_block);
    alloc->deallocate(first_block);
    alloc->deallocate(third_block);

    delete alloc;
}

TEST(allocatorSortedListPositiveTests, test2)
{

    logger* logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "a.txt",
                logger::severity::information
        }
    });

    allocator* alloc = new allocator_sorted_list(3000, nullptr, logger_instance,
        allocator_with_fit_mode::fit_mode::the_worst_fit);

    auto first_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 250));

    auto* the_same_subject = dynamic_cast<allocator_with_fit_mode*>(alloc);
    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::first_fit);
    auto second_block = reinterpret_cast<char*>(alloc->allocate(sizeof(char), 500));
    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_best_fit);
    auto third_block = reinterpret_cast<double*>(alloc->allocate(sizeof(double*), 100));

    alloc->deallocate(first_block);
    alloc->deallocate(second_block);
    alloc->deallocate(third_block);

    delete alloc;
}

TEST(allocatorSortedListPositiveTests, test3)
{
    logger* logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "c.txt",
                logger::severity::information
        }
    });

    logger* logger_instance2 = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "cx.txt",
                logger::severity::information
        }
    });
    size_t size = 1e5;
    allocator* alloc = new allocator_sorted_list(size, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);

    auto first_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 250));
    auto second_block = reinterpret_cast<char*>(alloc->allocate(sizeof(char), 500));
    auto third_block = reinterpret_cast<double*>(alloc->allocate(sizeof(double*), 250));

    alloc->deallocate(first_block);

    first_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 245));

    allocator* allocator = new allocator_sorted_list(size, nullptr, logger_instance2);
    auto* the_same_subject = dynamic_cast<allocator_with_fit_mode*>(alloc);
    int iterations_count = 1e4;

    std::list<void*> allocated_blocks;

    srand((unsigned)time(nullptr));

    for (auto i = 0; i < iterations_count; i++)
    {
        int deal_or_alloc = rand() % 5;

        if (deal_or_alloc < 2)
        {
            try
            {
                int fit_mode = rand() % 3;
                if (fit_mode == 2)
                {
                    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::first_fit);
                } else if (fit_mode == 2)
                {
                    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_best_fit);
                } else
                {
                    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_worst_fit);
                }

                allocated_blocks.push_front(allocator->allocate(sizeof(char), rand() % 2048 + 50));

            } catch (std::bad_alloc const& ex)
            {
                std::cout << i << ' ' << ex.what() << std::endl;
            }
        } else
        {
            if (allocated_blocks.empty())
            {
                //std::cout << "No blocks to deallocate" << std::endl;
                continue;
            }

            auto it = allocated_blocks.begin();
            std::advance(it, rand() % allocated_blocks.size());

            allocator->deallocate(*it);
            allocated_blocks.erase(it);

        }
    }

    while (!allocated_blocks.empty())
    {
        auto it = allocated_blocks.begin();
        std::advance(it, rand() % allocated_blocks.size());
        allocator->deallocate(*it);
        allocated_blocks.erase(it);
        std::cout << "deallocation succeeded" << std::endl;
    }


    delete allocator;
    delete alloc;
    delete logger_instance2;
    delete logger_instance;

}

TEST(allocatorSortedListPositiveTests, test4)
{
    //TODO: logger

    allocator* alloc = new allocator_sorted_list(1000, nullptr, nullptr, allocator_with_fit_mode::fit_mode::first_fit);

    auto first_block = reinterpret_cast<unsigned char*>(alloc->allocate(sizeof(unsigned char), 250));
    auto second_block = reinterpret_cast<unsigned char*>(alloc->allocate(sizeof(char), 150));
    auto third_block = reinterpret_cast<unsigned char*>(alloc->allocate(sizeof(unsigned char), 300));

    auto* the_same_subject = dynamic_cast<allocator_with_fit_mode*>(alloc);
    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_worst_fit);
    auto four_block = reinterpret_cast<unsigned char*>(alloc->allocate(sizeof(unsigned char), 50));

    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_best_fit);
    auto five_block = reinterpret_cast<unsigned char*>(alloc->allocate(sizeof(unsigned char), 50));

    alloc->deallocate(first_block);
    alloc->deallocate(second_block);
    alloc->deallocate(third_block);
    alloc->deallocate(four_block);
    alloc->deallocate(five_block);

    //TODO: проверка

    delete alloc;
}


TEST(allocatorSortedListNegativeTests, test1)
{
    logger* logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "allocator_sorted_list_tests_logs_negative_test_1.txt",
                logger::severity::information
        }
    });
    allocator* alloc = new allocator_sorted_list(3000, nullptr, logger, allocator_with_fit_mode::fit_mode::first_fit);

    ASSERT_THROW(alloc->allocate(sizeof(char), 3100), std::bad_alloc);

    delete alloc;
    delete logger;
}

int main(
    int argc,
    char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}