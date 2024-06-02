#include <gtest/gtest.h>
#include "../include/client_logger.h"
#include "../include/client_logger_builder.h"

#include <filesystem>

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    client_logger_builder builder;

    builder.add_file_stream("a.txt" ,logger::severity::trace)->
            add_file_stream("b.txt" ,logger::severity::trace)->
            add_file_stream("b.txt" ,logger::severity::trace)->
            add_file_stream("a.txt", logger::severity::debug)->
            add_file_stream("../logger/a.txt", logger::severity::debug)->
            add_console_stream(logger::severity::trace)->
            set_format("[%d %t][%s] %m");

    builder.transform_with_configuration("set.json", "log");

    std::unique_ptr<logger> log(builder.build());

    log->trace("it is a very long message!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")->debug("dd");

    std::unique_ptr<logger> logger2(builder.build());

    logger2->trace("From second logger");

    return RUN_ALL_TESTS();
}