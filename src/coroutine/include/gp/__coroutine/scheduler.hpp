#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <memory>
#include <list>
// System Library
// Third-Party Library
// Local Library
#include <gp/__coroutine/promise.hpp>

GP_BEGIN
GP_CORO_BEGIN

// Todo:
//  1. 用侵入式指针替代 std::list
class GP_CORO_API CoScheduler {
public:
    using Iterator = std::list<coro::CoHandle>::iterator;

    CoScheduler();
    ~CoScheduler();

    CoScheduler(const CoScheduler &) noexcept = delete;
    CoScheduler(CoScheduler &&) noexcept;

    auto operator=(const CoScheduler &rhs) noexcept -> CoScheduler & = delete;
    auto operator=(CoScheduler &&rhs) noexcept -> CoScheduler &;

    template <typename T>
        requires(std::is_base_of_v<coro::CoHandle, T>)
    auto pushBack(T &&handle) -> Iterator
    {
        handle.promise().scheduler(this);
        return _readyQueue.insert(_readyQueue.end(), std::forward<T &&>(handle));
    }

protected:
    std::list<coro::CoHandle> _readyQueue;
    // std::list<coro::CoHandle> _awaitQueue;
    // std::list<coro::CoHandle> _completeQueue;
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