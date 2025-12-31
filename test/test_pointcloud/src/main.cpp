#include <gp/config.hpp>
// Standard Library
#include <filesystem>
// System Library

// Third-Party Library
#include <tiny_gltf.h>
#include <spdlog/spdlog.h>
// Local Library
#include "argvcvt.hpp"

int main(int argc, const char *const *argvloc)
{
    // 全平台字符编码强制修改为 UTF-8
    auto [argu8, argvec, argstr] = argvloc2utf8(argc, argvloc);
    std::setlocale(LC_ALL, ".UTF-8");
    // 保存 运行路径 和 工作目录
    std::filesystem::path execPath = std::filesystem::weakly_canonical(argu8[0]);
    std::filesystem::path workDir  = execPath.parent_path().parent_path().parent_path();

    tinygltf::TinyGLTF loader{};
    tinygltf::Model    model{};
    std::string        errc{};
    std::string        warnc{};

    if (!loader.LoadASCIIFromFile(&model, &errc, &warnc,
                                  (workDir / "data" / "stanford-bunny.gltf").string())) {
        if (!errc.empty()) {
            SPDLOG_ERROR("Load Error, error info: {}", errc);
        }
        if (!warnc.empty()) {
            SPDLOG_WARN("Load Error, warn info: {}", warnc);
        }
        return 0;
    }

    SPDLOG_INFO("scene count: {}", model.meshes.size());
    for (std::size_t i = 0; i < model.meshes.size(); ++i) {
        auto &mesh = model.meshes.at(i);
    }
    return 0;
}