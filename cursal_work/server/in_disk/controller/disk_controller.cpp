#include "disk_controller.h"

generator<unsigned int> atomic_index_gen()
{
    std::atomic<unsigned int> num;

    while (true)
    {
        co_yield num.load();
        ++num;
    }
}