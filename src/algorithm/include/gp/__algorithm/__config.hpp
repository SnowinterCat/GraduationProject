#pragma once
#include <gp/config.hpp>
// Standard Library
#include <version>

// GP_ALGORITHM_API
#if defined(_WIN32) && defined(GP_SHARED_BUILD)
    #if defined(GP_ALGORITHM_COMPILING)
        #define GP_ALGORITHM_API __declspec(dllexport)
    #else
        #define GP_ALGORITHM_API __declspec(dllimport)
    #endif
#endif
#if !defined(GP_ALGORITHM_API)
    #if !defined(GP_NO_GCC_API_ATTRIBUTE) && defined(__GNUC__) && (__GNUC__ >= 4)
        #define GP_ALGORITHM_API __attribute__((visibility("default")))
    #else
        #define GP_ALGORITHM_API
    #endif
#endif

#define GP_ALGORITHM_BEGIN                                                                               \
    namespace alg                                                                                  \
    {
#define GP_ALGORITHM_END }

// clang-format off
#define GP_ALGORITHM_LOG_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define GP_ALGORITHM_LOG_DEBUG(...)    SPDLOG_DEBUG(__VA_ARGS__)
#define GP_ALGORITHM_LOG_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define GP_ALGORITHM_LOG_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define GP_ALGORITHM_LOG_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
#define GP_ALGORITHM_LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
// clang-format on
