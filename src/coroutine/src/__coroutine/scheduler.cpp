#include "gp/__coroutine/promise.hpp"
#include <gp/__coroutine/scheduler.hpp>
// Standard Library
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_CORO_BEGIN

CoScheduler::CoScheduler()
{
    _readyQueue._coPromise        = new coro::CoPromise();
    _awaitQueue._coPromise        = new coro::CoPromise();
    _completeQueue._coPromise     = new coro::CoPromise();
    _readyQueue.promise().next    = _readyQueue;
    _readyQueue.promise().pre     = _readyQueue;
    _awaitQueue.promise().next    = _awaitQueue;
    _awaitQueue.promise().pre     = _awaitQueue;
    _completeQueue.promise().next = _completeQueue;
    _completeQueue.promise().pre  = _completeQueue;
}
CoScheduler::~CoScheduler()
{
    delete _completeQueue._coPromise;
    delete _awaitQueue._coPromise;
    delete _readyQueue._coPromise;
}

CoScheduler::CoScheduler(CoScheduler &&rhs) noexcept
    : _readyQueue(std::exchange(rhs._readyQueue, {}))
{
}
auto CoScheduler::operator=(CoScheduler &&rhs) noexcept -> CoScheduler &
{
    this->_readyQueue = std::exchange(rhs._readyQueue, {});
    return *this;
}

void CoScheduler::ready(coro::CoHandle handle)
{
    _moveTo(_readyQueue, handle);
}
void CoScheduler::await(coro::CoHandle handle)
{
    _moveTo(_awaitQueue, handle);
}
void CoScheduler::complete(coro::CoHandle handle)
{
    _moveTo(_completeQueue, handle);
}

void CoScheduler::_moveTo(coro::CoHandle &queue, coro::CoHandle &handle)
{
    auto &pre          = handle.promise().pre;
    auto &next         = handle.promise().next;
    pre.promise().next = next;
    next.promise().pre = pre;

    handle.promise().next              = queue;
    handle.promise().pre               = queue.promise().pre;
    queue.promise().pre.promise().next = handle;
    queue.promise().pre                = handle;
}

void CoScheduler::resume()
{
    auto &handle = _readyQueue.promise().next;

    handle.promise().coroState = coro::CoState::eExecute;
    handle.handle().resume();
}

void SingleThreadQueueScheduler::debugPrint()
{
    // std::println("head: {}, tail: {}", static_cast<void *>(_readyQueueHead),
    //              static_cast<void *>(_readyQueueTail));
    // for (const auto *i = _readyQueueHead; i != nullptr; i = i->promise().next()) {
    //     std::println("handle: {}", static_cast<const void *>(i));
    // }
}

GP_CORO_END
GP_END