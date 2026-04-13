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

auto coro1() -> LocalTask<float>
{
    std::println("[coro1]hello world1");
    co_await std::suspend_always();
    std::println("[coro1]hello world2");
    co_return {std::error_code(), 1};
}

auto coro2() -> LocalTask<int>
{
    auto errc  = std::error_code();
    auto value = int();

    std::println("[coro2]hello world0, value: {}", static_cast<void *>(&value));
    std::tie(errc, value) = co_await coro1();
    std::println("[coro2]hello world1, value: {}", static_cast<void *>(&value));
    co_return {std::error_code(), 2};
}

auto coro3() -> LocalTask<int>
{
    auto errc  = std::error_code();
    auto value = int();

    std::println("[coro3]hello world");
    std::tie(errc, value) = co_await coro1();
    co_return {std::error_code(), 3};
}

auto main() -> int
{
    auto sched = gp::coro::CoScheduler();
    // auto handle1 = coro1();
    // auto handle3 = coro3();
    // std::println("handle2 addr: {}", handle2.handle().address());
    auto handle2 = sched.pushBack(coro2());
    // sched.push(std::move(handle2));
    // sched.push(std::move(handle3));

    // sched.debugPrint();

    handle2->resume();
    handle2->resume();
    return 0;
}