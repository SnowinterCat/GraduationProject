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
class CoScheduler {
public:
    CoScheduler();
    ~CoScheduler();

    template <typename T>
        requires(std::is_base_of_v<coro::CoHandle, T>)
    T push(T &handle)
    {
        handle.promise().scheduler() = this;
        return _readyQueuePushBack(handle);
    }

protected:
    template <typename T>
        requires(std::is_base_of_v<coro::CoHandle, T>)
    T _readyQueuePushBack(T &handle);

protected:
    // std::list<coro::CoHandle> _readyQueue;
    // std::list<coro::CoHandle> _awaitQueue;
    // std::list<coro::CoHandle> _completeQueue;
    coro::CoHandle _readyQueue;
};

class SingleThreadQueueScheduler : public CoScheduler {
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