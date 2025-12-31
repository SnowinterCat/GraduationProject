#pragma once
#include <gp/config.hpp>

// define GP_FORCE_EXPORT_API
#if defined(_WIN32)
    #define GP_FORCE_EXPORT_API __declspec(dllexport)
#endif
#if !defined(GP_FORCE_EXPORT_API)
    #if defined(__GNUC__) && (__GNUC__ >= 4)
        #define GP_FORCE_EXPORT_API __attribute__((visibility("default")))
    #else
        #define GP_FORCE_EXPORT_API
    #endif
#endif
