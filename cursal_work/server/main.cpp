//
// Created by Des Caldnd on 6/2/2024.
//
//#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_VERSION 5
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <boost/thread/future.hpp>
#include <iostream>
#include <crossguid/guid.hpp>

#include <thread>
#include <chrono>

using namespace std::chrono_literals;

int main()
{
    auto id = xg::newGuid();
    auto c = boost::async(boost::launch::async, [](){ return xg::newGuid(); });

    auto d = c.then([](auto x) {

        std::cout << x.get() << std::endl; });

    std::this_thread::sleep_for(100ms);
}