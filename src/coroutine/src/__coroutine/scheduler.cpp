#include "gp/__coroutine/promise.hpp"
#include <gp/__coroutine/scheduler.hpp>
// Standard Library
#include <print>
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_CORO_BEGIN

CoScheduler::CoScheduler()  = default;
CoScheduler::~CoScheduler() = default;

CoScheduler::CoScheduler(CoScheduler &&rhs) noexcept
    : _readyQueue(std::exchange(rhs._readyQueue, {})),
      _awaitQueue(std::exchange(rhs._awaitQueue, {})),
      _completeQueue(std::exchange(rhs._completeQueue, {})),
      _cancelQueue(std::exchange(rhs._cancelQueue, {})),
      _destoryQueue(std::exchange(rhs._destoryQueue, {}))
{
}
auto CoScheduler::operator=(CoScheduler &&rhs) noexcept -> CoScheduler &
{
    this->_readyQueue    = std::exchange(rhs._readyQueue, {});
    this->_awaitQueue    = std::exchange(rhs._awaitQueue, {});
    this->_completeQueue = std::exchange(rhs._completeQueue, {});
    this->_cancelQueue   = std::exchange(rhs._cancelQueue, {});
    this->_destoryQueue  = std::exchange(rhs._destoryQueue, {});
    return *this;
}

void CoScheduler::ready(coro::CoPromise *promise)
{
    promise->coroState = coro::CoState::eReady;
    promise->moveBefore(&_readyQueue);
}
void CoScheduler::await(coro::CoPromise *promise, uint32_t awaitCnt)
{
    promise->coroState = coro::CoState::eAwait;
    promise->awaitCnt  = awaitCnt;
    promise->moveBefore(&_awaitQueue);
}
void CoScheduler::complete(coro::CoPromise *promise)
{
    promise->coroState = coro::CoState::eComplete;
    promise->moveBefore(&_completeQueue);
}
void CoScheduler::cancel(coro::CoPromise *promise)
{
    promise->coroState = coro::CoState::eCancel;
    promise->moveBefore(&_cancelQueue);
}
void CoScheduler::destory(coro::CoPromise *promise)
{
    promise->coroState = coro::CoState::eDestory;
    promise->moveBefore(&_destoryQueue);
}

void CoScheduler::resume()
{
    auto *promise = _readyQueue.next;
    if (promise->handle != nullptr) {
        promise->coroState = coro::CoState::eExecute;
        promise->handle.resume();
    }
    while (_destoryQueue.next != &_destoryQueue) {
        auto *temp         = _destoryQueue.next;
        _destoryQueue.next = temp->next;
        temp->handle.destroy();
    }
}

GP_CORO_END
GP_END