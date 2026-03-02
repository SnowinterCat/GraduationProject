#include <gp/__coroutine/scheduler.hpp>
// Standard Library
// #include <print>
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_CORO_BEGIN

CoScheduler::CoScheduler()
{
    _readyQueue._coPromise      = new coro::CoPromise();
    _readyQueue.promise()._next = _readyQueue;
    _readyQueue.promise()._pre  = _readyQueue;
}
CoScheduler::~CoScheduler()
{
    for (auto &i = _readyQueue.promise()._next; i.handle() != nullptr; i = i.promise()._next) {
        i.handle().destroy();
    }
    delete _readyQueue._coPromise;
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

GP_CORO_END
GP_END