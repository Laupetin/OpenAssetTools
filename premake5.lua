require("premake", ">=5.0.0-beta5")

include "tools/scripts/folders.lua"
include "tools/scripts/including.lua"
include "tools/scripts/linking.lua"
include "tools/scripts/options.lua"
include "tools/scripts/platform.lua"
include "tools/scripts/version.lua"
include "tools/scripts/source_templating.lua"

-- ==================
-- Workspace
-- ==================
workspace "OpenAssetTools"
    location "build"
    objdir "%{wks.location}/obj"
    symbols "On"
    systemversion "latest"
    cppdialect "C++23"
    largeaddressaware "on"

	flags {
		"MultiProcessorCompile"
	}

    configurations { 
        "Debug",
        "Release" 
    }

    platforms {
        "x86",
        "x64"
    }
    defaultplatform "x86"

    disablewarnings {
        "26812" -- Prefer enum class over unscoped
    }

    filter "platforms:x86"
        architecture "x86"
        defines "ARCH_x86"
    filter {}

    filter "platforms:x64"
        architecture "x86_64"
        defines "ARCH_x64"
    filter {}

    filter "configurations:Debug"
        defines "_DEBUG"
        optimize "Debug"
        symbols "On"
    filter {}

    filter {"system:windows", "configurations:Debug" }
        buildoptions { "/bigobj" }
    filter {}

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "Full"
        symbols "Off"
        fatalwarnings { "All" }
    filter {}

    defines {
        "__STDC_LIB_EXT1__",
        "__STDC_WANT_LIB_EXT1__=1",
        "_CRT_SECURE_NO_WARNINGS"
    }

    -- Write the current version to a header
    -- This is better than adding it as macro here since changing a global macro would cause a full rebuild
    WriteVersionHeader()
    includedirs {
        GetVersionHeaderFolder()
    }
    
    filter "options:debug-structureddatadef"
        defines { "STRUCTUREDDATADEF_DEBUG" }
    filter {}
    filter "options:debug-techset"
        defines { "TECHSET_DEBUG" }
    filter {}
    filter "options:experimental-material-compilation"
        defines { "EXPERIMENTAL_MATERIAL_COMPILATION" }
    filter {}

-- ========================
-- ThirdParty
-- ========================
include "thirdparty/catch2.lua"
include "thirdparty/eigen.lua"
include "thirdparty/libtomcrypt.lua"
include "thirdparty/libtommath.lua"
include "thirdparty/lz4.lua"
include "thirdparty/lzx.lua"
include "thirdparty/json.lua"
include "thirdparty/minilzo.lua"
include "thirdparty/minizip.lua"
include "thirdparty/salsa20.lua"
include "thirdparty/webview.lua"
include "thirdparty/zlib.lua"

-- ThirdParty group: All projects that are external dependencies
group "ThirdParty"
    catch2:project()
    eigen:project()
    libtomcrypt:project()
    libtommath:project()
    lz4:project()
    lzx:project()
    json:project()
    minilzo:project()
    minizip:project()
    salsa20:project()
    zlib:project()
    
    if _OPTIONS["modman"] then
        webview:project()
    end
group ""

-- ========================
-- Projects
-- ========================
include "src/Common.lua"
include "src/Cryptography.lua"
include "src/ImageConverter.lua"
include "src/Linker.lua"
include "src/ModMan.lua"
include "src/Parser.lua"
include "src/RawTemplater.lua"
include "src/Unlinker.lua"
include "src/Utils.lua"
include "src/ZoneCode.lua"
include "src/ZoneCodeGeneratorLib.lua"
include "src/ZoneCodeGenerator.lua"
include "src/ZoneCommon.lua"
include "src/ZoneLoading.lua"
include "src/ZoneWriting.lua"
include "src/ZoneCommon.lua"
include "src/ObjCommon.lua"
include "src/ObjCompiling.lua"
include "src/ObjImage.lua"
include "src/ObjLoading.lua"
include "src/ObjWriting.lua"
include "tools/scripts/raw.lua"

-- Components group: All projects assist or are part of a tool
group "Components"
    Common:project()
    Cryptography:project()
    Parser:project()
    Utils:project()
    ZoneCode:project()
    ZoneCodeGeneratorLib:project()
    ZoneCommon:project()
    ZoneLoading:project()
    ZoneWriting:project()
    ObjCommon:project()
    ObjCompiling:project()
    ObjImage:project()
    ObjLoading:project()
    ObjWriting:project()
group ""

-- Tools group: All projects that compile into the final tools
group "BuildTools"
    RawTemplater:project()
    ZoneCodeGenerator:project()
group ""

-- Tools group: All projects that compile into the final tools
group "Tools"
    Linker:project()
    Unlinker:project()
    ImageConverter:project()

    if _OPTIONS["modman"] then
        ModMan:project()
    end
group ""

group "Raw"
    Raw:project()
group ""

-- ========================
-- Tests
-- ========================
include "test/Catch2Common.lua"
include "test/ObjCommonTestUtils.lua"
include "test/ObjCommonTests.lua"
include "test/ObjCompilingTests.lua"
include "test/ObjLoadingTests.lua"
include "test/ObjWritingTests.lua"
include "test/ParserTestUtils.lua"
include "test/ParserTests.lua"
include "test/ZoneCodeGeneratorLibTests.lua"
include "test/ZoneCommonTests.lua"

-- Tests group: Unit test and other tests projects
group "Tests"
    Catch2Common:project()
    ObjCommonTestUtils:project()
    ObjCommonTests:project()
    ObjCompilingTests:project()
    ObjLoadingTests:project()
    ObjWritingTests:project()
    ParserTestUtils:project()
    ParserTests:project()
    ZoneCodeGeneratorLibTests:project()
    ZoneCommonTests:project()
group ""
