//
// Created by Des Caldnd on 6/2/2024.
//
//#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_VERSION 5
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include <boost/thread/future.hpp>
#include <iostream>

int main()
{
    auto c = boost::async([](){ return 3; });

//    auto d = c.then([](auto x) {
//
//        return x.get() * 3;});

    std::cout << c.get();
}