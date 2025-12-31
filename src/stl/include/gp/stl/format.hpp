#pragma once
#include <gp/config.hpp>
#include <gp/__stl/__config.hpp>
// Standard Library
#include <version>
// fmtlib
#if __cpp_lib_format >= 202311L
    #include <format>
#else
    #include <fmt/xchar.h>
#endif

GP_BEGIN
GP_STL_BEGIN

namespace fmt
{
#if __cpp_lib_format >= 202311L
    using namespace ::std;
#else
    using namespace ::fmt;
#endif
} // namespace fmt

GP_STL_END
GP_END