//
// Created by Des Caldnd on 6/2/2024.
//
#include <iostream>

#include <thread>
#include <chrono>

#include <vector>

#include <memory_controller.h>
#include "../structure/student.h"


using namespace std::chrono_literals;

int main()
{
    B_tree<cw_string, student> d;
    memory_controller<cw_string, student> c;
}