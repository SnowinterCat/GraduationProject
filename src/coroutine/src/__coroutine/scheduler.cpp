#include <gp/__coroutine/scheduler.hpp>
// Standard Library
#include <memory>
#include <print>
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_CORO_BEGIN

CoScheduler::CoScheduler()
{
    _readyQueueHead = _readyQueueTail = new QueueUnit(nullptr, nullptr);
    _awaitQueueHead = _awaitQueueTail = new QueueUnit(nullptr, nullptr);
    _completeQueuHead = _completeQueueTail = new QueueUnit(nullptr, nullptr);
}
CoScheduler::~CoScheduler()
{
    for (auto *i = _readyQueueHead; i != nullptr; i = i->next) {
        if (i->handle != nullptr) {
            i->handle->handle().destroy();
        }
    }
    for (auto *i = _awaitQueueHead; i != nullptr; i = i->next) {
        if (i->handle != nullptr) {
            i->handle->handle().destroy();
        }
    }
    for (auto *i = _completeQueuHead; i != nullptr; i = i->next) {
        if (i->handle != nullptr) {
            i->handle->handle().destroy();
        }
    }
}

// void CoScheduler::push(coro::CoHandle &&handle)
// {
//     // handle.promise().scheduler() = this;
//     // // 加入 memQueue 管理生命周期
//     // _readyQueueHead if (_memQueue != nullptr) [[unlikely]]
//     // {
//     //     handle.promise()._memNext = std::move(_memQueue->promise()._memNext);
//     // }
//     // _memQueue = std::make_unique<coro::CoHandle>(std::move(handle));
//     // // 加入 readyQueue 准备调度
// }

void SingleThreadQueueScheduler::debugPrint()
{
    // std::println("head: {}, tail: {}", static_cast<void *>(_readyQueueHead),
    //              static_cast<void *>(_readyQueueTail));
    // for (const auto *i = _readyQueueHead; i != nullptr; i = i->promise().next()) {
    //     std::println("handle: {}", static_cast<const void *>(i));
    // }
}

void SingleThreadQueueScheduler::_readyQueuPush(std::unique_ptr<coro::CoHandle> &&handle)
{
    // std::println("handle addr: {}", static_cast<void *>(&handle));
    // if (_readyQueueTail != nullptr) [[likely]] {
    //     _readyQueueTail->promise().next() = &handle;
    // }
    // handle.promise().next() = nullptr;
    // _readyQueueTail         = &handle;
    // if (_readyQueueHead == nullptr) [[unlikely]] {
    //     _readyQueueHead = &handle;
    // }
}

GP_CORO_END
GP_END