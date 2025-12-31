#pragma once
#include <gp/config.hpp>
// Standard Library
#include <tuple>
#include <vector>

auto argvloc2utf8(int argc, const char *const *argv)
    -> std::tuple<const char *const *, std::vector<const char *>, std::vector<char>>;