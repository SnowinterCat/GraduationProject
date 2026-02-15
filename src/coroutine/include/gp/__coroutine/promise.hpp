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

class Promise {
public:
    auto initial_suspend() noexcept { return std::suspend_always(); }              // NOLINT
    auto final_suspend() noexcept { return std::suspend_always(); }                // NOLINT
    auto unhandled_exception() noexcept { _exception = std::current_exception(); } // NOLINT

protected:
    std::exception_ptr _exception = nullptr;
};

class Handle {
public:
protected:
    std::coroutine_handle<> _stdHandle;
    gp::coro::Promise      *_coroHandle;
};

GP_CORO_END
GP_END