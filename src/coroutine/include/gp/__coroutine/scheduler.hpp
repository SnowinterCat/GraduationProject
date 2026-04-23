#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <memory>
// System Library
// Third-Party Library
// Local Library
#include <gp/__coroutine/promise.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4275)
    #pragma warning(disable : 4251)
#endif

GP_BEGIN
GP_CORO_BEGIN

// Todo:
//  1. 用侵入式指针替代 std::list
class GP_CORO_API CoScheduler {
public:
    CoScheduler();
    ~CoScheduler();

    CoScheduler(const CoScheduler &) noexcept = delete;
    CoScheduler(CoScheduler &&) noexcept;

    auto operator=(const CoScheduler &rhs) noexcept -> CoScheduler & = delete;
    auto operator=(CoScheduler &&rhs) noexcept -> CoScheduler &;

    template <typename T>
        requires(std::is_base_of_v<coro::CoHandle, T>)
    auto push(T handle) -> T
    {
        handle.promise().scheduler(this);
        handle.promise().next = handle;
        handle.promise().pre  = handle;
        _moveTo(_readyQueue, handle);
        return handle;
    }

    void ready(coro::CoHandle handle);
    void await(coro::CoHandle handle);
    void complete(coro::CoHandle handle);

    void resume();

protected:
    static void _moveTo(coro::CoHandle &queue, coro::CoHandle &handle);

protected:
    coro::CoHandle _readyQueue;
    coro::CoHandle _awaitQueue;
    coro::CoHandle _completeQueue;
};

class GP_CORO_API SingleThreadQueueScheduler : public CoScheduler {
public:
    void debugPrint();

protected:
    void _readyQueuPush(std::unique_ptr<coro::CoHandle> &&handle);
    void _awaitQueuePush();
    void _completeQueuePush();

protected:
};

GP_CORO_END
GP_END

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif