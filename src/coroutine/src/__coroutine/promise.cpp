#include <gp/__coroutine/promise.hpp>
// Standard Library
#include <utility>
#include <cassert>
#include <coroutine>
#include <exception>
// System Library
// Third-Party Library
// Local Library
#include <gp/__coroutine/scheduler.hpp>

GP_BEGIN
GP_CORO_BEGIN

// CoHandle

CoHandle::CoHandle() noexcept  = default;
CoHandle::~CoHandle() noexcept = default;

// CoHandle::CoHandle(const CoHandle &rhs) noexcept = delete;
CoHandle::CoHandle(CoHandle &&rhs) noexcept : _handle(std::exchange(rhs._handle, {})) {}

// auto CoHandle::operator=(const CoHandle &rhs) noexcept -> CoHandle & = delete;
auto CoHandle::operator=(CoHandle &&rhs) noexcept -> CoHandle &
{
    this->_handle = std::exchange(rhs._handle, {});
    return *this;
}

// auto CoHandle::operator<=>(const coro::CoHandle &rhs) const -> std::strong_ordering
// {
//     return _address <=> rhs._address;
// }
// auto CoHandle::operator==(const coro::CoHandle &rhs) const -> bool
// {
//     return _address == rhs._address;
// }

CoHandle::CoHandle(std::nullptr_t) noexcept : _handle(nullptr) {}
auto CoHandle::operator==(const std::nullptr_t &rhs) const -> bool
{
    return _handle == rhs;
}

auto CoHandle::handle() -> StdHandleType &
{
    return _handle;
}
auto CoHandle::handle() const -> const StdHandleType &
{
    return _handle;
}

auto CoHandle::promise() -> PromiseType *
{
    assert((_handle != nullptr) && "this->_handle is nullptr, a logical error exists.");
    return &_handle.promise();
}
auto CoHandle::promise() const -> PromiseType const *
{
    assert((_handle != nullptr) && "this->_handle is nullptr, a logical error exists.");
    return &_handle.promise();
}

// void resume()
// {
//     _stdHandle.resume();
// }

// CoPromise

CoPromise::CoPromise() noexcept
    : handle(nullptr), exception(), scheduler(nullptr),           // data
      coroLock(), coroState(coro::CoState::eCreated), awaitCnt(), // sync
      parent(nullptr), child(nullptr), sibling(nullptr),          // call tree
      next(this), pre(this)                                       // queue
{
}
CoPromise::~CoPromise() noexcept = default;

// CoPromise::CoPromise(const CoPromise &) noexcept = delete;
CoPromise::CoPromise(CoPromise &&rhs) noexcept
    : handle(std::exchange(rhs.handle, {})), exception(std::exchange(rhs.exception, {})),
      scheduler(std::exchange(rhs.scheduler, {})), coroLock(std::exchange(rhs.coroLock, {})),
      coroState(std::exchange(rhs.coroState, {})), awaitCnt(std::exchange(rhs.awaitCnt, {})),
      parent(std::exchange(rhs.parent, {})), child(std::exchange(rhs.child, {})),
      sibling(std::exchange(rhs.sibling, {})), next(std::exchange(rhs.next, {})),
      pre(std::exchange(rhs.pre, {}))
{
}

// auto CoPromise::operator=(const CoPromise &rhs) noexcept -> CoPromise & = delete;
auto CoPromise::operator=(CoPromise &&rhs) noexcept -> CoPromise &
{
    this->handle    = std::exchange(rhs.handle, {});
    this->exception = std::exchange(rhs.exception, {});
    this->scheduler = std::exchange(rhs.scheduler, {});
    this->coroLock  = std::exchange(rhs.coroLock, {});
    this->coroState = std::exchange(rhs.coroState, {});
    this->awaitCnt  = std::exchange(rhs.awaitCnt, {});
    this->parent    = std::exchange(rhs.parent, {});
    this->child     = std::exchange(rhs.child, {});
    this->sibling   = std::exchange(rhs.sibling, {});
    this->next      = std::exchange(rhs.next, {});
    this->pre       = std::exchange(rhs.pre, {});
    return *this;
}

// auto CoPromise::scheduler() -> coro::CoScheduler &
// {
//     return *_scheduler;
// }
// auto CoPromise::scheduler() const -> coro::CoScheduler const &
// {
//     return *_scheduler;
// }
// void CoPromise::scheduler(coro::CoScheduler *scheduler) noexcept
// {
//     _scheduler = scheduler;
// }

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto CoPromise::initial_suspend() noexcept -> std::suspend_always
{
    return std::suspend_always{};
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto CoPromise::final_suspend() noexcept -> FinalSuspend
{
    return FinalSuspend{this};
}
void CoPromise::unhandled_exception() noexcept
{
    exception = std::current_exception();
}

void CoPromise::rethrowIfException() const
{
    if (exception) {
        std::rethrow_exception(exception);
    }
}

void CoPromise::moveBefore(CoPromise *position)
{
    { // 从原队列移出
        this->pre->next = this->next;
        this->next->pre = this->pre;
    }
    { // 放入新队列
        next                = position;
        pre                 = position->pre;
        position->pre->next = this;
        position->pre       = this;
    }
}

void FinalSuspend::await_suspend(std::coroutine_handle<> /*unused*/) const noexcept
{
    auto *sch = self->scheduler;
    sch->complete(self);
    if (self->parent != nullptr && self->parent->coroState == coro::CoState::eAwait) {
        --self->parent->awaitCnt == 0 ? sch->ready(self->parent) : void();
    }
}

GP_CORO_END
GP_END