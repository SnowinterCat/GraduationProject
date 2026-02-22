#include <gp/__coroutine/promise.hpp>
// Standard Library
#include <utility>
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_CORO_BEGIN

// CoPromise

CoPromise::CoPromise() noexcept  = default;
CoPromise::~CoPromise() noexcept = default;
CoPromise::CoPromise(CoPromise &&rhs) noexcept
    : _exception(std::exchange(rhs._exception, {})), _scheduler(std::exchange(rhs._scheduler, {}))
{
}

auto CoPromise::scheduler() -> coro::CoScheduler *&
{
    return _scheduler;
}
auto CoPromise::scheduler() const -> coro::CoScheduler *const &
{
    return _scheduler;
}

auto CoPromise::initial_suspend() noexcept -> std::suspend_always // NOLINT
{
    return std::suspend_always();
}
auto CoPromise::final_suspend() noexcept -> std::suspend_always // NOLINT
{
    return std::suspend_always();
}
void CoPromise::unhandled_exception() noexcept // NOLINT
{
    _exception = std::current_exception();
}

// CoHandle

// CoHandle::CoHandle() noexcept  = default;
CoHandle::~CoHandle() noexcept = default;
CoHandle::CoHandle(const CoHandle &rhs) noexcept
    : _stdHandle(rhs._stdHandle), _coPromise((rhs._coPromise))
{
}
CoHandle::CoHandle(CoHandle &&rhs) noexcept
    : _stdHandle(std::exchange(rhs._stdHandle, {})), _coPromise(rhs._coPromise)
{
}

// CoHandle::CoHandle(std::nullptr_t) noexcept : _stdHandle(nullptr), _coPromise(nullptr) {}

auto CoHandle::handle() -> std::coroutine_handle<> &
{
    return _stdHandle;
}
auto CoHandle::handle() const -> const std::coroutine_handle<> &
{
    return _stdHandle;
}

auto CoHandle::promise() -> PromiseType &
{
    return _coPromise;
}
auto CoHandle::promise() const -> PromiseType const &
{
    return _coPromise;
}

// void resume()
// {
//     _stdHandle.resume();
// }

GP_CORO_END
GP_END