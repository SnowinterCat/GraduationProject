#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <type_traits>
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
        // 移动所有权并销毁包装句柄
        std::remove_reference_t<T> temp = std::forward<T &&>(handle);
        // 用 Promise 维护
        auto *promise      = temp.promise();
        promise->handle    = temp.handle();
        promise->scheduler = this;
        ready(promise);
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