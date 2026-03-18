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
    (void)std::setlocale(LC_ALL, ".UTF-8"); // NOLINT(concurrency-*):Only Once In main
    // 保存 运行路径 和 工作目录
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::filesystem::path execPath = std::filesystem::weakly_canonical(argu8[0]);
    std::filesystem::path workDir  = execPath.parent_path().parent_path().parent_path();

    tinygltf::TinyGLTF loader{};
    tinygltf::Model    model{};
    std::string        errc{};
    std::string        warnc{};

    if (!loader.LoadASCIIFromFile(&model, &errc, &warnc,
                                  (workDir / "data" / "stanford-bunny.gltf").string())) {
        if (!errc.empty()) {
            SPDLOG_ERROR("Load gltf model Error, error info: {}", errc);
        }
        if (!warnc.empty()) {
            SPDLOG_WARN("Load gltf model Error, warn info: {}", warnc);
        }
        return -1;
    }

    for (auto &mesh : model.meshes) {
        for (auto &primitive : mesh.primitives) {
            primitive.mode    = TINYGLTF_MODE_POINTS;
            primitive.indices = -1;
        }
    }

    if (!loader.WriteGltfSceneToFile(&model,
                                     (workDir / "data" / "stanford-bunny-pointcloud.gltf").string(),
                                     true, true, true, true)) {
        SPDLOG_WARN("Write gltf model Error");
        return -2;
    }
    return 0;
}