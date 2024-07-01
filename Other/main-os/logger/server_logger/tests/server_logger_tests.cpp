#include <gtest/gtest.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "httplib.h"
#include "../include/server_logger_builder.h"

int main(
    int argc,
    char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
   

    server_logger_builder builder;
    builder.add_console_stream(logger::severity::debug)
        ->add_console_stream(logger::severity::trace);

    builder.add_file_stream(".txt", logger::severity::debug);
    logger *logger = builder.build();

    logger->log("test", logger::severity::trace);
    logger->log("test", logger::severity::trace); 
    logger->log("test", logger::severity::debug);
    for (int i = 0; i < 10; i++)
    {
        std::string s = "testn";
        s[4] = '0' + (i % 10);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        logger->log(s, logger::severity::trace);
        std::cout << s;
    }
    delete logger;
    return RUN_ALL_TESTS();
}