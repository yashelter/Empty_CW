cmake_minimum_required(VERSION 3.21)
project(cw_utility LANGUAGES CXX)


add_library(
        cw_utility
        generator.h
        crutch.cpp)
target_include_directories(
        cw_utility
        PUBLIC
        ./)
set_target_properties(
        cw_utility PROPERTIES
        LANGUAGES CXX
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        LINKER_LANGUAGE CXX
        CXX_EXTENSIONS OFF
        VERSION 1.0
        DESCRIPTION "server logger implementation library")