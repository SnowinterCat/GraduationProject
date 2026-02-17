#include <gp/config.hpp>
// Standard Library
#include <print>
// System Library
// Third-Party Library
// Local Library
#include <gp/coroutine.hpp>

gp::coro::LocalTaskHandle<int> coro1()
{
    std::println("hello world1");
    co_await std::suspend_always();
    std::println("hello world2");
    co_return 1;
}

int main()
{
    auto handle  = coro1();
    auto handle1 = handle;
    std::println("value: {}", handle.promise().value());
    handle.resume();
    std::println("value: {}", handle.promise().value());
    handle1.resume();
    std::println("value: {}", handle1.promise().value());
    return 0;
}