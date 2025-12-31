#pragma once
#include <gp/config.hpp>
#include <gp/__stl/__config.hpp>
// Standard Library
#include <cstdio>
#include <memory>

GP_BEGIN
GP_STL_BEGIN

struct GP_STL_API FileCloser {
    void operator()(FILE *file);
};

namespace raii
{
    using File = std::unique_ptr<FILE, FileCloser>;
}

GP_STL_END
GP_END