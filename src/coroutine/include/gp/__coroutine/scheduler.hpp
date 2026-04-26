#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
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
    auto push(T &&handle) -> T::PromiseType *
    {
        auto *promise      = handle.promise();
        promise->handle    = std::forward<T &&>(handle).handle();
        promise->scheduler = this;
        promise->moveBefore(&_readyQueue);
        return promise;
    }

    void ready(coro::CoPromise *promise);
    void await(coro::CoPromise *promise, uint32_t awaitCnt);
    void complete(coro::CoPromise *promise);
    void cancel(coro::CoPromise *promise);
    void destory(coro::CoPromise *promise);

    void resume();

protected:
    coro::CoPromise _readyQueue;
    coro::CoPromise _awaitQueue;
    coro::CoPromise _completeQueue;
    coro::CoPromise _cancelQueue;
    coro::CoPromise _destoryQueue;
};

GP_CORO_END
GP_END

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif