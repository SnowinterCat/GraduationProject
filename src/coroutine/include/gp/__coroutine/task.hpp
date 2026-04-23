#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <type_traits>
#include <optional>
// #include <list>
// #include <print>
// System Library
// Third-Party Library
// Local Library
#include <gp/__coroutine/promise.hpp>
#include <gp/__coroutine/scheduler.hpp>

GP_BEGIN
GP_CORO_BEGIN

template <typename>
class LocalTaskPromise;
template <typename>
class LocalTaskHandle;
template <typename>
class LocalTaskAwaiter;

template <typename T>
class LocalTaskPromise : public coro::CoPromise {
public:
    using PromiseType   = LocalTaskPromise<T>;
    using StdHandleType = std::coroutine_handle<PromiseType>;

    // NOLINTNEXTLINE(readability-identifier-naming)
    void return_value(T value) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        _value.emplace(std::move(value));
    }
    template <typename U>
    // NOLINTNEXTLINE(readability-identifier-naming)
    void return_value(U &&value) noexcept(std::is_nothrow_constructible_v<T, U>)
    {
        _value.emplace(std::forward<U>(value));
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    // auto return_void() noexcept {}

    // NOLINTNEXTLINE(readability-identifier-naming)
    auto get_return_object() noexcept -> LocalTaskHandle<T>
    {
        return static_cast<LocalTaskHandle<T>>(
            static_cast<StdHandleType>(StdHandleType::from_promise(*this)));
    }

    auto value() -> T & { return _value; }
    auto value() const -> T const & { return _value; }

    [[nodiscard]] auto getValue() -> T && { return std::move(*_value); }

protected:
    std::optional<T> _value;
};

template <typename T>
class LocalTaskHandle : public coro::CoHandle {
public:
    using PromiseType   = coro::LocalTaskPromise<T>;
    using StdHandleType = std::coroutine_handle<PromiseType>;

    using promise_type = PromiseType;

    LocalTaskHandle() noexcept                        = default;
    ~LocalTaskHandle() noexcept                       = default;
    LocalTaskHandle(const LocalTaskHandle &) noexcept = default;
    LocalTaskHandle(LocalTaskHandle &&) noexcept      = default;

    explicit LocalTaskHandle(std::nullptr_t) noexcept {}
    explicit LocalTaskHandle(StdHandleType handle) noexcept : coro::CoHandle(handle) {}

    auto operator=(const LocalTaskHandle &rhs) noexcept -> LocalTaskHandle & = default;
    auto operator=(LocalTaskHandle &&rhs) noexcept -> LocalTaskHandle &      = default;

    // auto handle() -> StdHandleType & { return _stdHandle; }
    // auto handle() const -> const StdHandleType & { return _stdHandle; }

    auto promise() -> PromiseType & { return static_cast<PromiseType &>(*_coPromise); }
    auto promise() const -> PromiseType const & { return static_cast<PromiseType &>(*_coPromise); }

    auto operator co_await() && noexcept -> coro::LocalTaskAwaiter<T>
    {
        return coro::LocalTaskAwaiter<T>(*this);
    }

protected:
};

template <typename T>
class LocalTaskAwaiter {
public:
    template <typename U>
    using PromiseType = coro::LocalTaskPromise<U>;
    template <typename U>
    using CoHandleType = coro::LocalTaskHandle<U>;
    template <typename U>
    using StdHandleType = std::coroutine_handle<PromiseType<U>>;

    explicit LocalTaskAwaiter(CoHandleType<T> &handle) : _handle(handle) {}

    // NOLINTNEXTLINE(readability-identifier-naming)
    [[nodiscard]] constexpr auto await_ready() const noexcept -> bool { return false; }

    template <typename U>
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr void await_suspend(StdHandleType<U> caller) noexcept
    {
        auto &sch = caller.promise().scheduler();

        _caller                     = static_cast<coro::CoHandle>(caller);
        _caller.promise().coroState = coro::CoState::eAwait;
        _caller.promise().awaitCnt  = 1;
        _caller.promise().child     = _handle;
        _handle.promise().parent    = _caller;
        _handle.promise().sibling   = nullptr;
        sch.await(_caller);
        _handle = sch.push(_handle);
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    [[nodiscard]] constexpr auto await_resume() noexcept -> T &&
    {
        return std::move(_handle.promise().getValue());
    }

protected:
    CoHandleType<T> _handle; // 当前协程函数
    coro::CoHandle  _caller; // call 当前协程函数的协程函数
};

GP_CORO_END
GP_END