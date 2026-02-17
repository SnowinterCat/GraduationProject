#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <type_traits>
// System Library
// Third-Party Library
// Local Library
#include <gp/__coroutine/promise.hpp>

GP_BEGIN
GP_CORO_BEGIN

template <typename>
class GP_CORO_API LocalTaskHandle;

template <typename T>
class GP_CORO_API LocalTaskPromise : public coro::CoPromise {
public:
    using PromiseType = LocalTaskPromise<T>;
    using HandleType  = std::coroutine_handle<PromiseType>;

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
            static_cast<HandleType>(HandleType::from_promise(*this)));
    }

    auto value() -> T & { return _value; }
    auto value() const -> const T & { return _value; }

protected:
    T _value;
};

template <typename T>
class GP_CORO_API LocalTaskHandle : public coro::CoHandle {
public:
    using PromiseType = coro::LocalTaskPromise<T>;
    using HandleType  = std::coroutine_handle<PromiseType>;

    using promise_type = PromiseType;

    LocalTaskHandle() noexcept                        = default;
    ~LocalTaskHandle() noexcept                       = default;
    LocalTaskHandle(const LocalTaskHandle &) noexcept = default;
    LocalTaskHandle(LocalTaskHandle &&) noexcept      = default;

    LocalTaskHandle(std::nullptr_t) noexcept {}
    LocalTaskHandle(std::coroutine_handle<PromiseType> handle) noexcept : coro::CoHandle(handle) {}

    auto handle() -> HandleType & { return _stdHandle; }
    auto handle() const -> const HandleType & { return _stdHandle; }

    auto promise() -> PromiseType & { return *static_cast<PromiseType *>(_coPromise); }
    auto promise() const -> const PromiseType & { return *static_cast<PromiseType *>(_coPromise); }

protected:
};

GP_CORO_END
GP_END