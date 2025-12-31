#pragma once
#include <version>

// C++ library
#ifdef _MSVC_STL_VERSION
    // msvc stl
    #define VKS_STL_MSVC
    #ifndef _CRT_SECURE_NO_WARNINGS
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#elif defined(_LIBCPP_VERSION)
    // libc++
    #define VKS_STL_LIBCXX
#elif defined(__GLIBCXX__)
    // libstdc++
    #define VKS_STL_STDCXX
#else
    #error "not support other stl"
#endif