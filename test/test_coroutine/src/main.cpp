#include <gp/config.hpp>
// Standard Library
#include <print>
// System Library
// Third-Party Library
// Local Library
#include <gp/coroutine.hpp>

gp::coro::LocalTask<int> coro1()
{
    std::println("hello world");
    co_return;
}

int main()
{
    auto handle = coro1();
    // handle._handle.resume();
    return 0;
}