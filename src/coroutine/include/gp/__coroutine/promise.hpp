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

class GP_CORO_API CoPromise {
public:
    CoPromise() noexcept                  = default;
    ~CoPromise() noexcept                 = default;
    CoPromise(const CoPromise &) noexcept = delete;
    CoPromise(CoPromise &&) noexcept      = default;

    auto initial_suspend() noexcept { return std::suspend_always(); }              // NOLINT
    auto final_suspend() noexcept { return std::suspend_always(); }                // NOLINT
    auto unhandled_exception() noexcept { _exception = std::current_exception(); } // NOLINT

protected:
    std::exception_ptr _exception = nullptr;
};

class GP_CORO_API CoHandle {
public:
    using PromiseType = coro::CoPromise;

    CoHandle() noexcept;
    ~CoHandle() noexcept;
    CoHandle(const CoHandle &) noexcept;
    CoHandle(CoHandle &&) noexcept;

    CoHandle(std::nullptr_t) noexcept;
    template <typename T>
        requires(std::is_base_of_v<CoPromise, T>)
    CoHandle(std::coroutine_handle<T> stdHandle) noexcept
        : _stdHandle(stdHandle), _coPromise(&stdHandle.promise())
    {
    }

    auto handle() -> std::coroutine_handle<> &;
    auto handle() const -> const std::coroutine_handle<> &;

    auto promise() -> PromiseType &;
    auto promise() const -> const PromiseType &;

    void resume() { _stdHandle.resume(); }

protected:
    std::coroutine_handle<> _stdHandle;
    coro::CoPromise        *_coPromise;
};

GP_CORO_END
GP_END