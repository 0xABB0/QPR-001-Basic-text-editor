-- Define project
set_project("SimpleTextEditor")
set_version("1.0.0")

-- Set project options
set_languages("c99")
set_warnings("all")

-- Enable Unicode support
add_defines("UNICODE", "_UNICODE")

-- Add target
target("SimpleTextEditor")
    -- Set target kind
    set_kind("binary")
    
    -- Set output directory
    set_targetdir("bin")
    
    -- Set toolchain to clang-cl
    set_toolchains("clang-cl")
    
    -- Add source files (accepts both .c and .cpp extensions)
    add_files("main.cpp")
    
    -- Add required Windows libraries
    add_syslinks("user32", "gdi32", "comdlg32", "comctl32")
    
    -- Add Windows subsystem
    add_ldflags("/SUBSYSTEM:WINDOWS", {force = true})
    
    -- Optimize for release builds
    if is_mode("release") then
        add_defines("NDEBUG")
        add_cxflags("/O2", {force = true})
    -- Add debug info for debug builds
    elseif is_mode("debug") then
        add_defines("DEBUG")
        add_cxflags("/Zi", {force = true})
        add_ldflags("/DEBUG", {force = true})
    end
    
    -- Enable Multi-processor compilation
    add_cxflags("/MP", {force = true})
    
    -- Set runtime library
    add_cxflags("/MD", {force = true})