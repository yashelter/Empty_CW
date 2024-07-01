#include <gtest/gtest.h>
#include <../include/client_logger_builder.h>
#include <../include/client_logger.h>



int main(
    int argc,
    char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    client_logger_builder builder = client_logger_builder();
    
    builder.add_console_stream(logger::severity::trace);

    builder.add_file_stream("out.txt", logger::severity::trace);
    builder.add_file_stream("dfasf", logger::severity::trace);
    builder.add_file_stream("riumuer", logger::severity::warning);
    builder.set_format("[%s] %m");
    builder.transform_with_configuration("config.json", "based_logger");

    client_logger *logger = builder.build();

    logger->log("test", logger::severity::trace);

    client_logger* logger2 = builder.build();
    delete logger;
    delete logger2;

    std::cout << "\nend of file\n";


    

    return RUN_ALL_TESTS();
}