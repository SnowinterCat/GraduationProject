target("GraduationProject")
    set_kind("binary")
    set_targetdir("$(bindir)")

    add_deps("config")
    add_packages(
        has_config("has_std_out_ptr") and nil or "out_ptr",
        has_config("has_std_expected") and nil or "tl_expected"
    )

    -- add_includedirs("include", {public = true})
    -- add_headerfiles("include/(**)")
    -- add_headerfiles("src/(**.hpp)", {install = false})
    add_files("src/**.cpp")
    add_syslinks("user32", "d3d11", "dxgi", "dxguid", "d3dcompiler")

    after_build(function (target) 
        import("lua.auto", {rootdir = os.projectdir()})
        auto().target_autoclean(target)
    end)
target_end()