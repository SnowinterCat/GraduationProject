#pragma once
#include <gp/config.hpp>
#include <gp/__stl/__config.hpp>
// Standard Library
#include <version>
// outptrlib
#if defined(__cpp_lib_out_ptr)
    #include <memory>
#else
    #include <memory>
    #include <ztd/out_ptr.hpp>
#endif

GP_BEGIN
GP_STL_BEGIN

namespace outptr
{
#if defined(__cpp_lib_out_ptr)
    using namespace ::std;
#else
    using namespace ::ztd::out_ptr;
#endif
} // namespace outptr

GP_STL_END
GP_END