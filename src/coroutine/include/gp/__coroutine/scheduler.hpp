#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <memory>
// System Library
// Third-Party Library
// Local Library
#include <gp/__coroutine/promise.hpp>

GP_BEGIN
GP_CORO_BEGIN

class CoScheduler {
public:
    CoScheduler();
    ~CoScheduler();

    template <typename T>
        requires(std::is_base_of_v<coro::CoHandle, T>)
    T push(T &&handle)
    {
        handle.promise().scheduler() = this;
        return handle;
    }

protected:
    struct QueueUnit {
        std::unique_ptr<coro::CoHandle> handle;
        QueueUnit                      *next;
    };
    QueueUnit *_readyQueueHead, *_readyQueueTail;
    QueueUnit *_awaitQueueHead, *_awaitQueueTail;
    QueueUnit *_completeQueuHead, *_completeQueueTail;
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