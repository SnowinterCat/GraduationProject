-- 工程设置
set_project("graduation-project")
set_version("0.0.1", {build = tostring(get_config("buildversion")), soname = true})
set_xmakever("3.0.0")
option("alias", {showmenu = false, default = "gp"}) -- 工程名缩写

set_configvar("LEGAL_COPYRIGHT", "Copyright (C) 2024 SnowinterCat")
set_configvar("PROJECT_NAME", "graduation-project")

-- 全局设置
option("stdc",   {showmenu = true, default = 23, values = {23}})
option("stdcxx", {showmenu = true, default = 23, values = {26, 23}})
function stdc()   return "c"   .. tostring(get_config("stdc"))   end
function stdcxx() return "c++" .. tostring(get_config("stdcxx")) end

set_languages(stdc(), stdcxx())
set_warnings("allextra")
set_encodings("utf-8")
set_exceptions("cxx")

-- 添加编译选项
add_rules("mode.release", "mode.debug", "mode.releasedbg", "mode.minsizerel")
add_rules("plugin.compile_commands.autoupdate", {lsp = "clangd", outputdir = ".vscode"})

-- 编译设置
option("3rd_kind",     {showmenu = true, default = get_config("kind"), values = {"static", "shared"}})
option("outputdir",    {showmenu = true, default = path.join(os.projectdir(), "bin"), type = "string"})
option("buildversion", {showmenu = true, default = 0, type = "number"})
option("replace",      {showmenu = true, default = false, type = "boolean"}) -- export 三方库时是否强制替换

includes("lua/check")
check_macros("has_std_out_ptr",  "__cpp_lib_out_ptr",  {languages = stdcxx(), includes = "memory"})
check_macros("has_std_expected", "__cpp_lib_expected", {languages = stdcxx(), includes = "expected"})
check_macros("has_std_runtime_format", "__cpp_lib_format >= 202311L", {languages = stdcxx(), includes = "format"})

-- 隐藏设置、隐藏目标、打包命令
includes("lua/hideoptions.lua")
includes("lua/hidetargets.lua")
-- includes("lua/pack.lua")

-- some of the third-party use our own configurations
add_repositories("myrepo 3rd", {rootdir = os.scriptdir()})
-- header-only libraries
if not has_config("has_std_out_ptr") then
    add_requires("out_ptr")
end
if not has_config("has_std_expected") then
    add_requires("tl_expected")
end
-- add_requires("cxxopts", {version = "x.x.x"})
-- add_requires("vulkan-memory-allocator-hpp", {version = "3.1.0"})
add_requires("tinygltf", {version = "x.x.x"})
-- normal libraries
if not has_config("has_std_runtime_format") then
    add_requires("fmt", {version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared"), header_only = false}})
end
-- if not is_plat("android") then
--     add_requires("vulkansdk")
-- end
-- add_requires("libsdl3", {version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared"), x11 = true, wayland = true}})
-- add_requires("imgui", {version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared")}})
add_requires("spdlog", {version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared"), header_only = false, fmt_external = not has_config("has_std_runtime_format"), std_format = has_config("has_std_runtime_format"), wchar = true, wchar_console = true}})
add_requires("eigen", {version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared")}})
-- normal libraries' dependencies configurations
-- add_requireconfs("**.vulkan-memory-allocator", {override = true, version = "3.1.0"}) -- from vulkan-memory-allocator-hpp
-- add_requireconfs("**.vulkan-headers", {override = true, version = "1.3.296"})        -- from vulkan-memory-allocator
-- add_requireconfs("**.libxext", {system = true}) -- from libsdl3
-- add_requireconfs("**.libx11", {system = true})  -- from libsdl3
-- add_requireconfs("**.wayland", {system = true}) -- from libsdl3
add_requireconfs("**.fmt", {override = true, version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared"), header_only = false}}) -- from spdlog
add_requireconfs("**.nlohmann_json", {override = true, version = "x.x.x"}) -- from tinygltf
add_requireconfs("**.stb", {override = true, version = "x.x.x"})           -- from tinygltf

-- subdirectories
includes("src/*/xmake.lua")
-- includes("exec/*/xmake.lua")
includes("test/*/xmake.lua")
