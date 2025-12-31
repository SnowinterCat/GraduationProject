#include "argvcvt.hpp"
// System Library
#ifdef _WIN32
    #include <Windows.h>
#endif
// Standard Library

auto argvloc2utf8([[maybe_unused]] int argc, [[maybe_unused]] const char *const *argv)
    -> std::tuple<const char *const *, std::vector<const char *>, std::vector<char>>
{
    auto argvec = std::vector<const char *>();
    auto temp   = std::vector<char>();
#ifdef _WIN32
    // Local -> Utf16
    auto wtemp = std::vector<wchar_t>();
    auto wlen  = int();
    for (int i = 0; i < argc; ++i) {
        wlen += MultiByteToWideChar(CP_ACP, NULL, argv[i], -1, NULL, NULL);
    }
    wtemp.resize(wlen);
    auto curwlen = int();
    for (int i = 0; i < argc; ++i) {
        curwlen += MultiByteToWideChar(CP_ACP, NULL, argv[i], -1, &wtemp[curwlen], wlen - curwlen);
    }
    // Utf16 -> Utf8
    auto len = WideCharToMultiByte(CP_UTF8, NULL, wtemp.data(), wlen, NULL, NULL, NULL, NULL);
    temp.resize(len);
    (void)WideCharToMultiByte(CP_UTF8, NULL, wtemp.data(), wlen, temp.data(), len, NULL, NULL);
    // argvec
    argvec.reserve(argc);
    argvec.push_back(temp.data());
    for (size_t i = 1; i < temp.size(); ++i) {
        if (temp[i - 1] == '\0') {
            argvec.push_back(&temp[i]);
        }
    }
    return std::tuple(argvec.data(), std::move(argvec), std::move(temp));
#endif
    return std::tuple(argv, std::move(argvec), std::move(temp));
}