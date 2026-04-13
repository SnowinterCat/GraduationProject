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

class GP_CORO_API CoHandle {
    friend class coro::CoScheduler;

public:
    using PromiseType   = coro::CoPromise;
    using StdHandleType = std::coroutine_handle<>;

    CoHandle() noexcept;
    ~CoHandle() noexcept = default;
    CoHandle(const CoHandle &) noexcept;
    CoHandle(CoHandle &&) noexcept;

    explicit CoHandle(std::nullptr_t) noexcept;

    template <typename T>
        requires(std::is_base_of_v<coro::CoPromise, T>)
    explicit CoHandle(std::coroutine_handle<T> stdHandle) noexcept
        : _stdHandle(stdHandle), _coPromise(&stdHandle.promise())
    {
    }

    auto operator=(const CoHandle &rhs) noexcept -> CoHandle &;
    auto operator=(CoHandle &&rhs) noexcept -> CoHandle &;

    [[nodiscard]] auto handle() -> StdHandleType &;
    [[nodiscard]] auto handle() const -> StdHandleType const &;

    [[nodiscard]] auto promise() -> PromiseType &;
    [[nodiscard]] auto promise() const -> PromiseType const &;

    void resume() { _stdHandle.resume(); }

protected:
    StdHandleType    _stdHandle;
    coro::CoPromise *_coPromise;
};

class GP_CORO_API CoPromise {
    friend class coro::CoScheduler;

public:
    CoPromise() noexcept;
    ~CoPromise() noexcept = default;
    CoPromise(const CoPromise &) noexcept;
    CoPromise(CoPromise &&) noexcept;

    auto operator=(const CoPromise &rhs) noexcept -> CoPromise &;
    auto operator=(CoPromise &&rhs) noexcept -> CoPromise &;

    [[nodiscard]]
    auto scheduler() -> coro::CoScheduler &;
    [[nodiscard]]
    auto scheduler() const -> coro::CoScheduler const &;
    void scheduler(coro::CoScheduler *scheduler) noexcept;

    // NOLINTNEXTLINE(readability-identifier-naming)
    auto initial_suspend() noexcept -> std::suspend_always;
    // NOLINTNEXTLINE(readability-identifier-naming)
    auto final_suspend() noexcept -> std::suspend_always;

    void unhandled_exception() noexcept; // NOLINT(readability-identifier-naming)

protected:
    std::exception_ptr _exception;
    coro::CoScheduler *_scheduler;
};

GP_CORO_END
GP_END