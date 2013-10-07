
solution "Ice Hockey"
  configurations { "Debug", "Release" }

  project "Ice Hockey"
    kind "WindowedApp"
    language "C++"
    location "build"

    files {
      "src/base.h",
      "src/config.h",
      "src/core.h",
      "src/core.cpp",
      "src/sys.h",
      "src/sys_glfw.cpp",
      "src/game.cpp"
    }

    if os.is("linux") then
      libdirs { os.findlib("glfw"), os.findlib("openal") }
      links { "glfw", "openal" }
    elseif os.is("windows") then
    elseif os.is("macosx") then
    end

    configuration "Debug"
       defines { "DEBUG" }
       flags { "Symbols" }

    configuration "Release"
       defines { "NDEBUG" }
       flags { "Optimize" }