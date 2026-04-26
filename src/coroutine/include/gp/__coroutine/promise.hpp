#pragma once
#include <gp/__coroutine/__config.hpp>
// Standard Library
#include <cstddef>
#include <cstdint>
#include <coroutine>
#include <exception>
#include <format>
// System Library
// Third-Party Library
// Local Library

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4275)
    #pragma warning(disable : 4251)
#endif

GP_BEGIN
GP_CORO_BEGIN

class GP_CORO_API  CoPromise;
class GP_CORO_API  CoHandle;
class GP_CORO_API  CoScheduler;
struct GP_CORO_API FinalSuspend;

enum class CoState : uint8_t
{
    eCreated  = 0x00,
    eReady    = 0x01,
    eExecute  = 0x02,
    eAwait    = 0x03,
    eComplete = 0x04,
    eCancel   = 0x05,
    eDestory  = 0x06,
};

class GP_CORO_API CoHandle {
public:
    using PromiseType   = coro::CoPromise;
    using StdHandleType = std::coroutine_handle<coro::CoPromise>;
    using StdHandleVoid = std::coroutine_handle<>;

    CoHandle() noexcept;
    ~CoHandle() noexcept;

    CoHandle(const CoHandle &) noexcept = delete;
    CoHandle(CoHandle &&) noexcept;

    auto operator=(const CoHandle &rhs) noexcept -> CoHandle & = delete;
    auto operator=(CoHandle &&rhs) noexcept -> CoHandle &;
    auto operator<=>(const coro::CoHandle &rhs) const -> std::strong_ordering = default;

    explicit CoHandle(std::nullptr_t) noexcept;
    auto operator==(const std::nullptr_t &rhs) const -> bool;

    template <typename T>
        requires(std::is_base_of_v<coro::CoPromise, T>)
    explicit CoHandle(std::coroutine_handle<T> stdHandle) noexcept
        : _handle(StdHandleType::from_address(stdHandle.address()))
    {
    }

    [[nodiscard]] auto handle() -> StdHandleType &;
    [[nodiscard]] auto handle() const -> StdHandleType const &;

    [[nodiscard]] auto promise() -> PromiseType *;
    [[nodiscard]] auto promise() const -> PromiseType const *;

protected:
    StdHandleType _handle;
};

class GP_CORO_API CoPromise {
public:
    using HandleType    = CoHandle;
    using StdHandleType = std::coroutine_handle<coro::CoPromise>;
    using StdHandleVoid = std::coroutine_handle<>;

    CoPromise() noexcept;
    ~CoPromise() noexcept;

    CoPromise(const CoPromise &) noexcept = delete;
    CoPromise(CoPromise &&) noexcept;

    auto operator=(const CoPromise &rhs) noexcept -> CoPromise & = delete;
    auto operator=(CoPromise &&rhs) noexcept -> CoPromise &;

    // NOLINTNEXTLINE(readability-identifier-naming)
    auto initial_suspend() noexcept -> std::suspend_always;
    // NOLINTNEXTLINE(readability-identifier-naming)
    auto final_suspend() noexcept -> FinalSuspend;
    // NOLINTNEXTLINE(readability-identifier-naming)
    void unhandled_exception() noexcept;

    void rethrowIfException() const;

    void moveBefore(CoPromise *position);
    void moveBehind(CoPromise *position);

protected:
public:
    StdHandleVoid      handle;
    std::exception_ptr exception;
    coro::CoScheduler *scheduler;
    uint8_t            coroLock;
    coro::CoState      coroState;
    uint32_t           awaitCnt;
    coro::CoPromise   *parent;
    coro::CoPromise   *child;
    coro::CoPromise   *sibling;
    coro::CoPromise   *next;
    coro::CoPromise   *pre;
};

struct FinalSuspend {
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static,readability-identifier-naming)
    [[nodiscard]] constexpr auto await_ready() const noexcept -> bool { return false; }
    // NOLINTNEXTLINE(readability-identifier-naming)
    void await_suspend(std::coroutine_handle<>) const noexcept;
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr void await_resume() const noexcept {}

public:
    coro::CoPromise *self;
};

GP_CORO_END
GP_END

template <>
struct std::formatter<gp::coro::CoState> {
    constexpr auto parse(auto &ctxt) { return ctxt.begin(); }

    auto format(const auto &type, auto &ctxt) const
    {
        switch (type) {
        case gp::coro::CoState::eCreated:
            return std::format_to(ctxt.out(), "Created");
        case gp::coro::CoState::eReady:
            return std::format_to(ctxt.out(), "Ready");
        case gp::coro::CoState::eExecute:
            return std::format_to(ctxt.out(), "Execute");
        case gp::coro::CoState::eAwait:
            return std::format_to(ctxt.out(), "Await");
        case gp::coro::CoState::eComplete:
            return std::format_to(ctxt.out(), "Complete");
        case gp::coro::CoState::eCancel:
            return std::format_to(ctxt.out(), "Cancel");
        case gp::coro::CoState::eDestory:
            return std::format_to(ctxt.out(), "Destory");
        }
    }
};

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif