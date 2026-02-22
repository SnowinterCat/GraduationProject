#include <gp/config.hpp>
// Standard Library
#include <system_error>
#include <print>
// System Library
// Third-Party Library
// Local Library
#include <gp/coroutine.hpp>

template <typename T>
using LocalTask = gp::coro::LocalTaskHandle<std::tuple<std::error_code, T>>;

LocalTask<float> coro1()
{
    std::println("[coro1]hello world1");
    co_await std::suspend_always();
    std::println("[coro1]hello world2");
    co_return {std::error_code(), 1};
}

LocalTask<int> coro2()
{
    auto errc  = std::error_code();
    auto value = int();

    std::println("[coro2]hello world");
    std::tie(errc, value) = co_await coro1();
    co_return {std::error_code(), 2};
}

LocalTask<int> coro3()
{
    // auto errc  = std::error_code();
    // auto value = int();

    std::println("[coro3]hello world");
    // std::tie(errc, value) = co_await coro1();
    co_return {std::error_code(), 3};
}

int main()
{
    auto sched = gp::coro::SingleThreadQueueScheduler();
    // auto handle1 = coro1();
    auto handle2 = coro2();
    // auto handle3 = coro3();
    std::println("handle2 addr: {}", handle2.handle().address());
    // sched.push(std::move(handle2));
    // sched.push(std::move(handle2));
    // sched.push(std::move(handle3));

    // sched.debugPrint();

    handle2.resume();
    return 0;
}