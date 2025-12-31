#pragma once
#include <gp/config.hpp>
#include <gp/__stl/__config.hpp>
// Standard Library
#include <version>
// expectedlib
#if defined(__cpp_lib_expected)
    #include <expected>
#else
    #include <tl/expected.hpp>
#endif

GP_BEGIN
GP_STL_BEGIN

namespace expected
{
#if defined(__cpp_lib_expected)
    using namespace ::std;
#else
    using namespace ::tl;
#endif
} // namespace expected

GP_STL_END
GP_END