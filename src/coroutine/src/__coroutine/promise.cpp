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

CoHandle::CoHandle() noexcept : _stdHandle(), _coPromise() {}
// CoHandle::~CoHandle() noexcept                   = default;
CoHandle::CoHandle(const CoHandle &rhs) noexcept = default;
CoHandle::CoHandle(CoHandle &&rhs) noexcept
    : _stdHandle(std::exchange(rhs._stdHandle, {})), _coPromise(std::exchange(rhs._coPromise, {}))
{
}

CoHandle::CoHandle(std::nullptr_t) noexcept : _stdHandle(nullptr), _coPromise(nullptr) {}

auto CoHandle::operator=(const CoHandle &rhs) noexcept -> CoHandle & = default;
auto CoHandle::operator=(CoHandle &&rhs) noexcept -> CoHandle &
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

CoPromise::CoPromise() noexcept : _exception(), _scheduler() {}
// CoPromise::~CoPromise() noexcept                 = default;
CoPromise::CoPromise(const CoPromise &) noexcept = default;
CoPromise::CoPromise(CoPromise &&rhs) noexcept
    : _exception(std::exchange(rhs._exception, {})), _scheduler(std::exchange(rhs._scheduler, {}))
{
}

auto CoPromise::operator=(const CoPromise &rhs) noexcept -> CoPromise & = default;
auto CoPromise::operator=(CoPromise &&rhs) noexcept -> CoPromise &
{
    this->_exception = std::exchange(rhs._exception, {});
    this->_scheduler = std::exchange(rhs._scheduler, {});
    return *this;
}

auto CoPromise::scheduler() -> coro::CoScheduler &
{
    return *_scheduler;
}
auto CoPromise::scheduler() const -> coro::CoScheduler const &
{
    return *_scheduler;
}
void CoPromise::scheduler(coro::CoScheduler *scheduler) noexcept
{
    _scheduler = scheduler;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto CoPromise::initial_suspend() noexcept -> std::suspend_always
{
    return std::suspend_always{};
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto CoPromise::final_suspend() noexcept -> std::suspend_always
{
    return std::suspend_always{};
}
void CoPromise::unhandled_exception() noexcept
{
    _exception = std::current_exception();
}

GP_CORO_END
GP_END