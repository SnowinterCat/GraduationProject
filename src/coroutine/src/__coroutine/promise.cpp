#include <gp/__coroutine/promise.hpp>
// Standard Library
// System Library
// Third-Party Library
// Local Library

GP_BEGIN
GP_CORO_BEGIN

CoHandle::CoHandle() noexcept                 = default;
CoHandle::~CoHandle() noexcept                = default;
CoHandle::CoHandle(const CoHandle &) noexcept = default;
CoHandle::CoHandle(CoHandle &&) noexcept      = default;

CoHandle::CoHandle(std::nullptr_t) noexcept : _stdHandle(nullptr), _coPromise(nullptr) {}

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
    return *_coPromise;
}
auto CoHandle::promise() const -> const PromiseType &
{
    return *_coPromise;
}

// void resume()
// {
//     _stdHandle.resume();
// }

GP_CORO_END
GP_END