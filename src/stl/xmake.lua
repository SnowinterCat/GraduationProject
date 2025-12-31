target("stl")
    set_kind("$(kind)")
    set_targetdir("$(libdir)")

    add_deps("config")
    add_packages(
        has_config("has_std_out_ptr") and nil or "out_ptr",
        has_config("has_std_expected") and nil or "tl_expected",
        has_config("has_std_runtime_format") and nil or "fmt"
    )

    add_includedirs("include", {public = true})
    add_headerfiles("include/(**)")
    -- add_headerfiles("src/(**.hpp)", {install = false})
    add_files("src/**.cpp")

    on_load(function (target) 
        import("lua.auto", {rootdir = os.projectdir()})
        auto().target_autoname(target)
        auto().library_autodefine(target)
    end)
    after_build(function (target) 
        import("lua.auto", {rootdir = os.projectdir()})
        auto().target_autoclean(target)
    end)
target_end()