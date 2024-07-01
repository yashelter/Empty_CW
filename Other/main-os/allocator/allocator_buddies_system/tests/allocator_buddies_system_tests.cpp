#include <gtest/gtest.h>
#include <cmath>
#include <allocator.h>
#include <allocator_buddies_system.h>
#include <client_logger_builder.h>
#include <logger.h>
#include <list>
#include <logger_builder.h>

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

TEST(positiveTests, test1)
{
    logger* logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "a.txt",
                logger::severity::information
        }
    });


    allocator* allocator_instance = new allocator_buddies_system(12, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);

    size_t size_free = 0;
    auto actual_blocks_state = dynamic_cast<allocator_test_utils*>(allocator_instance)->get_blocks_info(size_free);
    std::vector<allocator_test_utils::block_info> expected_blocks_state
    {
        {.block_size = 4096, .is_block_occupied = false }
    };

    ASSERT_EQ(actual_blocks_state.size(), expected_blocks_state.size());
    for (int i = 0; i < actual_blocks_state.size(); i++)
    {
        ASSERT_EQ(actual_blocks_state[i], expected_blocks_state[i]);
    }

    delete allocator_instance;
    delete logger_instance;
}

TEST(positiveTests, test2)
{
    logger* logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "b.txt",
                logger::severity::information
        }
    });

    allocator* allocator_instance = new allocator_buddies_system(8, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);

    void* first_block = allocator_instance->allocate(sizeof(unsigned char), 40);

    size_t size_free = 0;
    auto actual_blocks_state = dynamic_cast<allocator_test_utils*>(allocator_instance)->get_blocks_info(size_free);
    std::vector<allocator_test_utils::block_info> expected_blocks_state
    {
        {.block_size = 64, .is_block_occupied = true },
        {.block_size = 64, .is_block_occupied = false },
        {.block_size = 128, .is_block_occupied = false }
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
    logger* logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "d.txt",
                logger::severity::information
        }
    });

    allocator* allocator_instance = new allocator_buddies_system(8, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);

    void* first_block = allocator_instance->allocate(sizeof(unsigned char), 0);
    void* second_block = allocator_instance->allocate(sizeof(unsigned char), 0);
    allocator_instance->deallocate(first_block);

    size_t size_free = 0;
    auto actual_blocks_state = dynamic_cast<allocator_test_utils*>(allocator_instance)->get_blocks_info(size_free);
    ASSERT_EQ(actual_blocks_state.size(), 5);

    ASSERT_EQ(actual_blocks_state[0].block_size, 1 << (static_cast<int>(std::floor(std::log2(sizeof(allocator::block_pointer_t) /** 2*/ + 1))) + 1));
    ASSERT_EQ(actual_blocks_state[0].is_block_occupied, false);
    ASSERT_EQ(actual_blocks_state[0].block_size, actual_blocks_state[1].block_size);
    ASSERT_EQ(actual_blocks_state[1].is_block_occupied, true);

    allocator_instance->deallocate(second_block);

    delete allocator_instance;
}

TEST(positiveTests, best_test)
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
    allocator* alloc = new allocator_buddies_system(12, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);

    auto first_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 250));
    auto second_block = reinterpret_cast<char*>(alloc->allocate(sizeof(char), 500));
    auto third_block = reinterpret_cast<double*>(alloc->allocate(sizeof(double*), 250));

    alloc->deallocate(first_block);

    first_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 245));

    allocator* allocator = new allocator_buddies_system(20, nullptr, logger_instance2);
    auto* the_same_subject = dynamic_cast<allocator_with_fit_mode*>(alloc);
    int iterations_count = 1e3;

    std::list<void*> allocated_blocks;

    srand((unsigned)time(nullptr));

    for (auto i = 0; i < iterations_count; i++)
    {
        int deal_or_alloc = rand() % 2;

        if (deal_or_alloc)
        {
            try
            {
                int fit_mode = rand() % 3;
                if (fit_mode == 2)
                {
                    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::first_fit);
                }
                else if (fit_mode == 2)
                {
                    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_best_fit);
                }
                else
                {
                    the_same_subject->set_fit_mode(allocator_with_fit_mode::fit_mode::the_worst_fit);
                }
               
                allocated_blocks.push_front(allocator->allocate(sizeof(char), rand() % 1024 + 50));

            } catch (std::bad_alloc const& ex)
            {
                std::cout << i << ' ' << ex.what() << std::endl;
            }
        }
        else
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

TEST(falsePositiveTests, test1)
{
    ASSERT_THROW(new allocator_buddies_system(static_cast<int>(std::floor(std::log2(sizeof(allocator::block_pointer_t) * 2 + 1))) - 1), std::logic_error);
}

int main(
    int argc,
    char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}