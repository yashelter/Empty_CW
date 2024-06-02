#include <gtest/gtest.h>
#include <cmath>
#include <allocator.h>
#include <allocator_buddies_system.h>
#include <client_logger_builder.h>
#include <logger.h>
#include <logger_builder.h>
#include <list>


logger *create_logger(
    std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
    bool use_console_stream = true,
    logger::severity console_stream_severity = logger::severity::debug)
{
    logger_builder *logger_builder_instance = new client_logger_builder;
    
    if (use_console_stream)
    {
        logger_builder_instance->add_console_stream(console_stream_severity);
    }
    
    for (auto &output_file_stream_setup: output_file_streams_setup)
    {
        logger_builder_instance->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
    }
    
    logger *logger_instance = logger_builder_instance->build();
    
    delete logger_builder_instance;
    
    return logger_instance;
}

TEST(positiveTests, test1)
{
    logger *logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "allocator_buddies_system_positiveTests_test1.txt",
                logger::severity::information
            }
        });
    allocator *allocator_instance = new allocator_buddies_system(12, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);
    
    auto actual_blocks_state = dynamic_cast<allocator_test_utils *>(allocator_instance)->get_blocks_info();
    std::vector<allocator_test_utils::block_info> expected_blocks_state
        {
            { .block_size = 4096, .is_block_occupied = false }
        };
    
    ASSERT_EQ(actual_blocks_state.size(), expected_blocks_state.size());
    for (int i = 0; i < actual_blocks_state.size(); i++)
    {
        ASSERT_EQ(actual_blocks_state[i], expected_blocks_state[i]);
    }
    
    delete allocator_instance;
    delete logger_instance;
}

TEST(positiveTests, test23)
{
    logger *logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "allocator_buddies_system_positiveTests_test1.txt",
                logger::severity::information
            }
        });
    allocator *allocator_instance = new allocator_buddies_system(8, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);
    
    void *first_block = allocator_instance->allocate(sizeof(unsigned char), 40);
    
    auto actual_blocks_state = dynamic_cast<allocator_test_utils *>(allocator_instance)->get_blocks_info();
    std::vector<allocator_test_utils::block_info> expected_blocks_state
        {
            { .block_size = 64, .is_block_occupied = true },
            { .block_size = 64, .is_block_occupied = false },
            { .block_size = 128, .is_block_occupied = false }
        };
    
    ASSERT_EQ(actual_blocks_state.size(), expected_blocks_state.size());
    for (int i = 0; i < actual_blocks_state.size(); i++)
    {
        ASSERT_EQ(actual_blocks_state[i], expected_blocks_state[i]);
    }
    
    allocator_instance->deallocate(first_block);
    
    delete allocator_instance;
    delete logger_instance;
}

TEST(positiveTests, test3)
{
    allocator *allocator_instance = new allocator_buddies_system(8, nullptr, nullptr, allocator_with_fit_mode::fit_mode::first_fit);
    
    void *first_block = allocator_instance->allocate(sizeof(unsigned char), 0);
    void *second_block = allocator_instance->allocate(sizeof(unsigned char), 0);
    allocator_instance->deallocate(first_block);
    
    auto actual_blocks_state = dynamic_cast<allocator_test_utils *>(allocator_instance)->get_blocks_info();
    ASSERT_EQ(actual_blocks_state.size(), 5);
    ASSERT_EQ(actual_blocks_state[0].block_size, 1 << (static_cast<int>(std::floor(std::log2(sizeof(allocator::block_pointer_t) + 1))) + 1));
    ASSERT_EQ(actual_blocks_state[0].is_block_occupied, false);
    ASSERT_EQ(actual_blocks_state[0].block_size, actual_blocks_state[1].block_size);
    ASSERT_EQ(actual_blocks_state[1].is_block_occupied, true);
    
    allocator_instance->deallocate(second_block);
    
    delete allocator_instance;
}

TEST(positiveTests, test53)
{
    logger *logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                    {
                                                            {
                                                                    "a.txt",
                                                                    logger::severity::information
                                                            },
                                                            {
                                                                    "a.txt",
                                                                    logger::severity::debug
                                                            },
                                                            {
                                                                    "a.txt",
                                                                    logger::severity::trace
                                                            },
                                                            {
                                                                    "a.txt",
                                                                    logger::severity::warning
                                                            },
                                                            {
                                                                    "a.txt",
                                                                    logger::severity::error
                                                            },
                                                            {
                                                                    "a.txt",
                                                                    logger::severity::critical
                                                            }
                                                    });

    allocator *alloc = new allocator_buddies_system(12, nullptr, logger_instance,
                                                    allocator_with_fit_mode::fit_mode::first_fit);

    auto first_block = reinterpret_cast<int *>(alloc->allocate(sizeof(int), 250));
    auto second_block = reinterpret_cast<char *>(alloc->allocate(sizeof(char), 500));
    auto third_block = reinterpret_cast<double *>(alloc->allocate(sizeof(double *), 250));
    alloc->deallocate(first_block);
    first_block = reinterpret_cast<int *>(alloc->allocate(sizeof(int), 245));

    //TODO: logger
    allocator *allocator = new allocator_buddies_system(13, nullptr, logger_instance,
                                                        allocator_with_fit_mode::fit_mode::first_fit);
    auto *the_same_subject = dynamic_cast<allocator_with_fit_mode *>(alloc);
    int iterations_count = 100;

    std::list<void *> allocated_blocks;
    srand((unsigned) time(nullptr));

    for (auto i = 0; i < iterations_count; i++)
    {
        switch (rand() % 2)
        {
            case 0:
            case 1:
                try
                {
                    switch (rand() % 2)
                    {
                        case 0:
                            the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::first_fit);
                        case 1:
                            the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_best_fit);
                        case 2:
                            the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_worst_fit);
                    }

                    allocated_blocks.push_front(allocator->allocate(sizeof(void *), rand() % 251 + 50));
                    std::cout << "allocation succeeded" << std::endl;
                }
                catch (std::bad_alloc const &ex)
                {
                    std::cout << ex.what() << std::endl;
                }
                break;
            case 2:
                if (allocated_blocks.empty())
                {
                    std::cout << "No blocks to deallocate" << std::endl;

                    break;
                }

                auto it = allocated_blocks.begin();
                std::advance(it, rand() % allocated_blocks.size());
                allocator->deallocate(*it);
                allocated_blocks.erase(it);
                std::cout << "deallocation succeeded" << std::endl;
                break;
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

    //TODO: проверка

    delete allocator;
    // delete logger;

    delete alloc;
}

TEST(falsePositiveTests, test1)
{
    ASSERT_THROW(new allocator_buddies_system(static_cast<int>(std::floor(std::log2(sizeof(allocator::block_pointer_t) * 2 + 1))) - 1), std::logic_error);
}

int main(
    int argc,
    char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}