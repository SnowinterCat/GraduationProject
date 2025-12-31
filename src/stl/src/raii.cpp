#include <gp/__stl/raii.hpp>

GP_BEGIN
GP_STL_BEGIN

void FileCloser::operator()(FILE *file)
{
    if (file != nullptr) {
        std::fclose(file);
    }
}

GP_STL_END
GP_END