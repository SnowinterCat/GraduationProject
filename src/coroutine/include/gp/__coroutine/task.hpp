#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <type_traits>
#include <print>
// System Library
// Third-Party Library
// Local Library
#include <gp/__coroutine/promise.hpp>

GP_BEGIN
GP_CORO_BEGIN

template <typename>
class GP_CORO_API LocalTaskPromise;
template <typename>
class GP_CORO_API LocalTaskHandle;
template <typename>
class GP_CORO_API LocalTaskAwaiter;

template <typename T>
class GP_CORO_API LocalTaskPromise : public coro::CoPromise {
public:
    using PromiseType   = LocalTaskPromise<T>;
    using StdHandleType = std::coroutine_handle<PromiseType>;

    auto return_value(T value) noexcept(std::is_nothrow_move_constructible_v<T>) // NOLINT
    {
        _value = std::move(value);
    }
    template <typename U>
    auto return_value(U &&value) noexcept(std::is_nothrow_constructible_v<T, U>) // NOLINT
    {
        _value = std::forward<U>(value);
    }

    // auto return_void() noexcept {} // NOLINT

    auto get_return_object() noexcept -> LocalTaskHandle<T> // NOLINT
    {
        return static_cast<LocalTaskHandle<T>>(
            static_cast<StdHandleType>(StdHandleType::from_promise(*this)));
    }

    auto value() -> T & { return _value; }
    auto value() const -> T const & { return _value; }

protected:
    T _value;
};

template <typename T>
class GP_CORO_API LocalTaskHandle : public coro::CoHandle {
public:
    using PromiseType   = coro::LocalTaskPromise<T>;
    using StdHandleType = std::coroutine_handle<PromiseType>;

    using promise_type = PromiseType;

    // LocalTaskHandle() noexcept  = default;
    ~LocalTaskHandle() noexcept                       = default;
    LocalTaskHandle(const LocalTaskHandle &) noexcept = default;
    LocalTaskHandle(LocalTaskHandle &&) noexcept      = default;

    // LocalTaskHandle(std::nullptr_t) noexcept {}
    LocalTaskHandle(StdHandleType handle) noexcept : coro::CoHandle(handle) {}

    auto handle() -> StdHandleType & { return _stdHandle; }
    auto handle() const -> const StdHandleType & { return _stdHandle; }

    auto promise() -> PromiseType & { return static_cast<PromiseType &>(_coPromise); }
    auto promise() const -> PromiseType const & { return static_cast<PromiseType &>(_coPromise); }

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
    using StdHandleType = std::coroutine_handle<PromiseType<U>>;
    template <typename U>
    using CoHandleType = coro::LocalTaskHandle<U>;

    LocalTaskAwaiter(CoHandleType<T> &handle) : _handle(handle) {}

    constexpr bool await_ready() const noexcept // NOLINT
    {
        return false;
    }
    template <typename U>
    constexpr void await_suspend(StdHandleType<U> caller) noexcept // NOLINT
    {
        // _caller = CoHandleType<U>(caller);
        auto sch = caller.promise().scheduler(); //->push(_handle);
        // caller 进入 awaitQueue，等待的函数数量增加一
        // sch.push();
        // handle 进入 readyQueue
        // sch.

        std::println("caller: {}", caller.address());
        std::println("this: {}", _handle.handle().address());
    }
    constexpr auto await_resume() noexcept -> T && // NOLINT
    {
        std::println("{}", typeid(T).name());
        // return T{};
        return std::move(_handle.promise().value());
    }

protected:
    coro::LocalTaskHandle<T> _handle;
    coro::CoHandle          *_caller;
};

GP_CORO_END
GP_END