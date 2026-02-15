#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
// System Library
// Third-Party Library
// Local Library
#include <gp/__coroutine/promise.hpp>

GP_BEGIN
GP_CORO_BEGIN

template <typename>
class LocalTask;

template <typename T>
class LocalTaskPromise : public gp::coro::Promise {
public:
    using handle_type = std::coroutine_handle<LocalTaskPromise<T>>;

    // auto return_value(T value) noexcept(std::is_nothrow_move_constructible_v<T>) // NOLINT
    // {
    //     _value.emplace(std::move(value));
    // }
    // template <typename U>
    // auto return_value(U &&value) noexcept(std::is_nothrow_constructible_v<T, U>) // NOLINT
    // {
    //     _value.emplace(std::forward<U>(value));
    // }

    auto return_void() noexcept {} // NOLINT

    auto get_return_object() noexcept -> LocalTask<T> // NOLINT
    {
        return static_cast<LocalTask<T>>(handle_type::from_promise(*this));
    }

protected:
    T _value;
};

template <typename T>
class LocalTask {
public:
    using promise_type = gp::coro::LocalTaskPromise<T>;
    using handle_type  = std::coroutine_handle<promise_type>;

// protected:
    handle_type _handle = std::noop_coroutine();
};

GP_CORO_END
GP_END