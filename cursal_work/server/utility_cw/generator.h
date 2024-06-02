//
// Created by Des Caldnd on 6/3/2024.
//

#ifndef MP_OS_GENERATOR_H
#define MP_OS_GENERATOR_H
#include <coroutine>
#include <cstdlib>

template <typename T>
struct generator
{
    struct promise_type
    {
        using coro_handle = std::coroutine_handle<promise_type>;

        auto get_return_object() { return coro_handle::from_promise(*this); }

        auto initial_suspend() { return std::suspend_always(); }
        auto final_suspend() noexcept { return std::suspend_always(); }
        void return_void(){}
        void unhandled_exception() { std::abort(); }

        T val;

        auto yield_value(T value){
            val = value;
            return std::suspend_always();
        }
    };

    using coro_handle = std::coroutine_handle<promise_type>;

    coro_handle _handle;

    generator(coro_handle handle) : _handle(handle){}

    bool resume() {
        if (!_handle.done()) _handle.resume();
        return !_handle.done();
    }

    T get() { return _handle.promise().val; }

    T operator()() {
        if (!_handle.done()) _handle.resume();
        return get(); }

    generator(const generator&) = delete;
    generator& operator=(const generator&) = delete;
    generator(generator&&) noexcept = default;
    generator& operator=(generator&&) noexcept = default;
    ~generator() { _handle.destroy(); }
};


#endif //MP_OS_GENERATOR_H
