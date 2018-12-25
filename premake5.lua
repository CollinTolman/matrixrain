workspace "MatrixRain"
    architecture "x86_64"
    system "windows"
    configurations { "Debug", "Release" }

    filter "system:windows"
        systemversion "latest"


project "MatrixRain"
    kind "WindowedApp"
    location "MatrixRain"
    language "C++"

    objdir "bin-int/%{cfg.buildcfg}/%{prj.name}"
    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"

    files { "%{prj.name}/**.h", "%{prj.name}/*.cpp" }

    includedirs { "vendor/SFML/include" }
    libdirs { "vendor/SFML/lib" }
    defines { "SFML_STATIC" }

    filter "configurations:Debug"
        defines { "_DEBUG" }
        symbols "On"
        optimize "Off"
        links { "sfml-system-s-d", "sfml-graphics-s-d", "sfml-window-s-d", "freetype", "opengl32", "gdi32", "winmm"  }

    filter "configurations:Release"
        defines { "NDEBUG" }
        symbols "Off"
        optimize "On"
        links { "sfml-system-s", "sfml-graphics-s", "sfml-window-s", "freetype", "opengl32", "gdi32", "winmm"  }
        postbuildcommands {
            "{COPY} res/ ../bin/%{cfg.buildcfg}/%{prj.name}/res"
        }