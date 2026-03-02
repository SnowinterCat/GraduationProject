#include <gp/__coroutine/promise.hpp>
// Standard Library
#include <utility>
#include <cassert>
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_CORO_BEGIN

// CoHandle

CoHandle::CoHandle() noexcept  = default;
CoHandle::~CoHandle() noexcept = default;
CoHandle::CoHandle(const CoHandle &rhs) noexcept
    : _stdHandle(rhs._stdHandle), _coPromise(rhs._coPromise)
{
}
CoHandle::CoHandle(CoHandle &&rhs) noexcept
    : _stdHandle(std::exchange(rhs._stdHandle, {})), _coPromise(std::exchange(rhs._coPromise, {}))
{
}

CoHandle::CoHandle(std::nullptr_t) noexcept : _stdHandle(nullptr), _coPromise(nullptr) {}

CoHandle &CoHandle::operator=(const CoHandle &rhs) noexcept = default;
CoHandle &CoHandle::operator=(CoHandle &&rhs) noexcept
{
    this->_stdHandle = std::exchange(rhs._stdHandle, {});
    this->_coPromise = std::exchange(rhs._coPromise, {});
    return *this;
}

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
    assert((_coPromise != nullptr) && "this->_coPromise is nullptr, a logical error exists.");
    return *_coPromise;
}
auto CoHandle::promise() const -> PromiseType const &
{
    assert((_coPromise != nullptr) && "this->_coPromise is nullptr, a logical error exists.");
    return *_coPromise;
}

// void resume()
// {
//     _stdHandle.resume();
// }

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

GP_CORO_END
GP_END