project("RED4.RTTIDumper")
    targetdir(paths.build())

    kind("SharedLib")
    language("C++")
    pchheader("stdafx.hpp")
    pchsource("stdafx.cpp")

    includedirs(
    {
        ".",
        paths.deps("json", "single_include"),
        paths.deps("red4ext.sdk", "include")
    })

    files(
    {
        "**.cpp",
        "**.hpp"
    })
