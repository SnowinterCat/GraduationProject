#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <coroutine>
#include <exception>
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_CORO_BEGIN

class GP_CORO_API CoPromise;
class GP_CORO_API CoHandle;
class GP_CORO_API CoScheduler;

class GP_CORO_API CoPromise {
    friend class coro::CoScheduler;

public:
    CoPromise() noexcept;
    ~CoPromise() noexcept;
    CoPromise(const CoPromise &) noexcept = delete;
    CoPromise(CoPromise &&) noexcept;

    auto scheduler() -> coro::CoScheduler *&;
    auto scheduler() const -> coro::CoScheduler *const &;

    auto initial_suspend() noexcept -> std::suspend_always; // NOLINT
    auto final_suspend() noexcept -> std::suspend_always;   // NOLINT

    void unhandled_exception() noexcept; // NOLINT

protected:
    std::exception_ptr _exception;
    coro::CoScheduler *_scheduler;
};

class GP_CORO_API CoHandle {
public:
    using PromiseType   = coro::CoPromise;
    using StdHandleType = std::coroutine_handle<>;

    // CoHandle() noexcept;
    ~CoHandle() noexcept;
    CoHandle(const CoHandle &) noexcept;
    CoHandle(CoHandle &&) noexcept;

    // CoHandle(std::nullptr_t) noexcept;

    template <typename T>
        requires(std::is_base_of_v<coro::CoPromise, T>)
    CoHandle(std::coroutine_handle<T> stdHandle) noexcept
        : _stdHandle(stdHandle), _coPromise(stdHandle.promise())
    {
    }

    auto handle() -> StdHandleType &;
    auto handle() const -> StdHandleType const &;

    auto promise() -> PromiseType &;
    auto promise() const -> PromiseType const &;

    void resume() { _stdHandle.resume(); }

protected:
    StdHandleType    _stdHandle;
    coro::CoPromise &_coPromise;
};

GP_CORO_END
GP_END