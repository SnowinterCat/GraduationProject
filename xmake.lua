-- 工程设置
set_project("graduation-project")
set_version("0.0.1", {build = tostring(get_config("buildversion")), soname = true})
set_xmakever("3.0.0")
option("alias", {showmenu = false, default = "gp"}) -- 工程名缩写

set_configvar("LEGAL_COPYRIGHT", "Copyright (C) 2024 SnowinterCat")
set_configvar("PROJECT_NAME", "graduation-project")

-- 全局设置
option("stdc",   {showmenu = true, default = 23, values = {23}})
option("stdcxx", {showmenu = true, default = 26, values = {26, 23}})
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
option("3rd_custom",   {showmenu = true, default = false, type = "boolean"})
option("3rd_kind",     {showmenu = true, default = get_config("kind"), values = {"static", "shared"}})
option("3rd_mode",     {showmenu = true, default = "release", values = {"release", "debug"}})
option("outputdir",    {showmenu = true, default = path.join(os.projectdir(), "bin"), type = "string"})
option("buildversion", {showmenu = true, default = 0, type = "number"})

includes("lua/check")
check_macros("has_std_out_ptr",         "__cpp_lib_out_ptr",            {languages = stdcxx(), includes = "version"})
check_macros("has_std_expected",        "__cpp_lib_expected",           {languages = stdcxx(), includes = "version"})
check_macros("has_std_runtime_format",  "__cpp_lib_format >= 202311L",  {languages = stdcxx(), includes = "version"})

-- 隐藏设置、隐藏目标、打包命令
includes("lua/hideoptions.lua")
includes("lua/hidetargets.lua")
-- includes("lua/pack.lua")

-- some of the required libraries use our own repository
add_repositories("myrepo 3rd", {rootdir = os.scriptdir()})
-- detected libraries
-- if not is_plat("android") then add_requires("vulkansdk") end
-- header-only libraries
if not has_config("has_std_out_ptr") then add_requires("out_ptr") end
if not has_config("has_std_expected") then add_requires("tl_expected") end
add_requires("tinygltf", "eigen"--[[, "vulkan-memory-allocator-hpp", "cxxopts"]])
-- normal libraries
if not has_config("has_std_runtime_format") then add_requires("fmt") end
add_requires("spdlog"--[[, "libsdl3", "imgui"]])


if not has_config("3rd_custom") then
-- configurations of required libraries
add_requireconfs("**out_ptr",       {override = true, version = "x.x.x"})
add_requireconfs("**tl_expected",   {override = true, version = "x.x.x"})
add_requireconfs("**tinygltf",      {override = true, version = "x.x.x"})
add_requireconfs("**eigen",         {override = true, version = "x.x.x"})
-- add_requireconfs("**vulkan-memory-allocator-hpp", {override = true, version = "3.1.0"})
-- add_requireconfs("**cxxopts",       {override = true, version = "x.x.x"})
add_requireconfs("**fmt",           {override = true, version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared"), debug = is_config("3rd_mode", "debug"), header_only = false}})
add_requireconfs("**spdlog",        {override = true, version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared"), debug = is_config("3rd_mode", "debug"), header_only = false, fmt_external = not has_config("has_std_runtime_format"), std_format = has_config("has_std_runtime_format"), wchar = true, wchar_console = true}})
-- add_requireconfs("**libsdl3",       {override = true, version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared"), debug = is_config("3rd_mode", "debug"), x11 = true, wayland = true}})
-- add_requireconfs("**imgui",         {override = true, version = "x.x.x", configs = {shared = is_config("3rd_kind", "shared"), debug = is_config("3rd_mode", "debug")}})
-- configurations of dependency libraries
add_requireconfs("**nlohmann_json", {override = true, version = "x.x.x"})   -- from tinygltf
add_requireconfs("**stb",           {override = true, version = "x.x.x"})   -- from tinygltf
-- add_requireconfs("**vulkan-memory-allocator", {override = true, version = "3.1.0"}) -- from vulkan-memory-allocator-hpp
-- add_requireconfs("**libxext", {system = true})  -- from libsdl3
-- add_requireconfs("**libx11",  {system = true})  -- from libsdl3
-- add_requireconfs("**wayland", {system = true})  -- from libsdl3
end

-- subdirectories
includes("src/*/xmake.lua")
-- includes("exec/*/xmake.lua")
includes("test/*/xmake.lua")
