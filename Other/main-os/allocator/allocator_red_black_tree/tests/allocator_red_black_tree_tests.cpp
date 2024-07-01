#include <gtest/gtest.h>
#include <logger.h>
#include <logger_builder.h>
#include <client_logger_builder.h>
#include <list>
#include <allocator_red_black_tree.h>

logger* create_logger(
    std::vector<std::pair<std::string, logger::severity>> const& output_file_streams_setup,
    bool use_console_stream = true,
    logger::severity console_stream_severity = logger::severity::debug)
{
    logger_builder* builder = new client_logger_builder();

    builder->set_format("[%t %d][%s] %m");

    for (auto& output_file_stream_setup : output_file_streams_setup)
    {
        builder->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
        builder->add_console_stream(output_file_stream_setup.second);
    }

    logger* built_logger = builder->build();

    delete builder;

    return built_logger;
}

TEST(allocatorRBTPositiveTests, test1)
{
    //TODO: logger
    logger* logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "allocator_boundary_tags_tests_logs_false_positive_test_1.txt",
                logger::severity::information
        }
   
    });

    allocator* alloc = new allocator_red_black_tree(3000, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);

    auto first_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 250));

    auto second_block = reinterpret_cast<char*>(alloc->allocate(sizeof(int), 250));
    alloc->deallocate(first_block);

    first_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 229));

    auto third_block = reinterpret_cast<int*>(alloc->allocate(sizeof(int), 250));

    alloc->deallocate(second_block);
    alloc->deallocate(first_block);
    alloc->deallocate(third_block);

    //    auto first = reinterpret_cast<char*>(alloc->allocate(sizeof(char), 10));
    //    auto second = reinterpret_cast<char*>(alloc->allocate(sizeof(char), 20));
    //    auto third = reinterpret_cast<char*>(alloc->allocate(sizeof(char), 30));
    //    auto fourth = reinterpret_cast<char*>(alloc->allocate(sizeof(char), 40));
    //    alloc->deallocate(second);
    //
    //    auto fifth = reinterpret_cast<char*>(alloc->allocate(sizeof(char), 50));
    //    alloc->deallocate(fourth);


    delete alloc;
}

TEST(allocatorRBTPositiveTests, test5)
{
    logger* logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "allocator_boundary_tags_tests_logs_false_positive_test_1.txt",
                logger::severity::information
        },
                                                            {
                                                                    "allocator_boundary_tags_tests_logs_false_positive_test_1.txt",
                                                                    logger::severity::debug
                                                            },
                                                            {
                                                                    "allocator_boundary_tags_tests_logs_false_positive_test_1.txt",
                                                                    logger::severity::trace
                                                            },
                                                            {
                                                                    "allocator_boundary_tags_tests_logs_false_positive_test_1.txt",
                                                                    logger::severity::critical
                                                            },
                                                            {
                                                                    "allocator_boundary_tags_tests_logs_false_positive_test_1.txt",
                                                                    logger::severity::warning
                                                            },
                                                            {
                                                                    "allocator_boundary_tags_tests_logs_false_positive_test_1.txt",
                                                                    logger::severity::error
                                                            }
    });


    //TODO: logger
    allocator_red_black_tree* allocator = new allocator_red_black_tree(20000, nullptr, nullptr, allocator_with_fit_mode::fit_mode::first_fit);
    int iterations_count = 1000;

    std::list<void*> allocated_blocks;
    srand((unsigned)time(nullptr));

    for (auto i = 0; i < iterations_count; i++)
    {
        switch (rand() % 3)
        {
        case 0:
        case 1:
            try
            {
                switch (rand() % 3)
                {
                case 0:
                    allocator->set_fit_mode(allocator_with_fit_mode::fit_mode::first_fit);
                    break;
                case 1:
                    allocator->set_fit_mode(allocator_with_fit_mode::fit_mode::the_best_fit);
                    break;
                case 2:
                    allocator->set_fit_mode(allocator_with_fit_mode::fit_mode::the_worst_fit);
                    break;
                }

                allocated_blocks.push_front(allocator->allocate(sizeof(char), rand() % 251 + 50));
                std::cout << "allocation succeeded" << std::endl;
            } catch (std::bad_alloc const& ex)
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

    //TODO: ????????

    delete allocator;
    delete logger_instance;
}


TEST(allocatorRBTPositiveTests, test7)
{
    logger* logger_instance = create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        {
            "allocator_boundary_tags_tests_logs_false_positive_test_1.txt",
                logger::severity::information
        }
    });


    //TODO: logger
    allocator_red_black_tree* allocator = new allocator_red_black_tree(7500, nullptr, logger_instance, allocator_with_fit_mode::fit_mode::first_fit);
    int iterations_count = 10000;

    void* first = allocator->allocate(1, 286);
    void* second = allocator->allocate(1, 226);
    void* third = allocator->allocate(1, 221);
    void* fourth = allocator->allocate(1, 274);
    void* fifth = allocator->allocate(1, 71);

    allocator->deallocate(second);

    void* six = allocator->allocate(1, 128);
    void* seven = allocator->allocate(1, 174);
    void* eight = allocator->allocate(1, 76);

    allocator->deallocate(first);
    allocator->deallocate(six);

    void* ten = allocator->allocate(1, 201);

    void* eleven = allocator->allocate(1, 234);


    delete allocator;
    delete logger_instance;
}


int main(
    int argc,
    char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}