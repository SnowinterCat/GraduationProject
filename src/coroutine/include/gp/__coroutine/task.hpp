#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <cstddef>
#include <type_traits>
#include <optional>
#include <coroutine>
// #include <list>
// #include <print>
// System Library
// Third-Party Library
// Local Library
#include <gp/__coroutine/promise.hpp>
#include <gp/__coroutine/scheduler.hpp>
#include <utility>

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
    using StdHandleVoid = std::coroutine_handle<>;

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

    auto value() -> std::optional<T> & { return _value; }
    auto value() const -> std::optional<T> const & { return _value; }

    [[nodiscard]] auto getValue() -> T &&
    {
        rethrowIfException();
        return std::move(*_value);
    }

protected:
    std::optional<T> _value;
};

template <typename T>
class LocalTaskHandle : public coro::CoHandle {
public:
    using PromiseType   = coro::LocalTaskPromise<T>;
    using StdHandleType = std::coroutine_handle<PromiseType>;
    using StdHandleVoid = std::coroutine_handle<>;

    using promise_type = PromiseType;

    LocalTaskHandle() noexcept  = delete;
    ~LocalTaskHandle() noexcept = default;

    LocalTaskHandle(const LocalTaskHandle &) noexcept = delete;
    LocalTaskHandle(LocalTaskHandle &&rhs) noexcept : coro::CoHandle(std::move(rhs)) {}

    auto operator=(const LocalTaskHandle &rhs) noexcept -> LocalTaskHandle & = delete;
    auto operator=(LocalTaskHandle &&rhs) noexcept -> LocalTaskHandle &
    {
        coro::CoHandle::operator=(std::move(rhs));
        return *this;
    }

    explicit LocalTaskHandle(std::nullptr_t) noexcept {}
    explicit LocalTaskHandle(StdHandleType handle) noexcept : coro::CoHandle(handle) {}

    // auto handle() -> StdHandleType & { return _stdHandle; }
    // auto handle() const -> const StdHandleType & { return _stdHandle; }

    auto promise() -> PromiseType *
    {
        return &StdHandleType::from_address(_handle.address()).promise();
    }
    auto promise() const -> PromiseType const *
    {
        return &StdHandleType::from_address(_handle.address()).promise();
    }

    auto operator co_await() && noexcept -> coro::LocalTaskAwaiter<T>
    {
        return coro::LocalTaskAwaiter<T>(std::move(*this));
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

    explicit LocalTaskAwaiter(CoHandleType<T> &&handle) : _handle(std::move(handle)) {}

    // NOLINTNEXTLINE(readability-identifier-naming)
    [[nodiscard]] constexpr auto await_ready() const noexcept -> bool { return false; }

    template <typename U>
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr void await_suspend(StdHandleType<U> caller) noexcept
    {
        // 加锁？
        auto *sch           = caller.promise().scheduler;
        auto *callerPromise = &caller.promise();
        _promise            = sch->push(std::move(_handle));
        // caller: Execute -> await
        // handle: Create -> ready
        sch->await(callerPromise, 1);
        sch->ready(_promise);
        // calling tree
        callerPromise->child = _promise;
        _promise->parent     = callerPromise;
        _promise->sibling    = nullptr;
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    [[nodiscard]] constexpr auto await_resume() noexcept -> T &&
    {
        _promise->rethrowIfException();
        _promise->scheduler->destory(_promise);
        return std::move(_promise->getValue());
    }

protected:
    CoHandleType<T>               _handle;    // 暂存当前协程函数
    CoHandleType<T>::PromiseType *_promise{}; // 当前协程 Promise
};

GP_CORO_END
GP_END