-- Functions for locating commonly used folders
local _ThirdPartyFolder = path.getabsolute("thirdparty")
function ThirdPartyFolder()
	return path.getrelative(os.getcwd(), _ThirdPartyFolder)
end

local _ProjectFolder = path.getabsolute("src")
function ProjectFolder()
	return path.getrelative(os.getcwd(), _ProjectFolder)
end

local _TestFolder = path.getabsolute("test")
function TestFolder()
	return path.getrelative(os.getcwd(), _TestFolder)
end

-- Target Directories
TargetDirectoryBin = "%{wks.location}/bin/%{cfg.buildcfg}_%{cfg.platform}"
TargetDirectoryLib = "%{wks.location}/lib/%{cfg.buildcfg}_%{cfg.platform}"
TargetDirectoryTest = "%{wks.location}/lib/%{cfg.buildcfg}_%{cfg.platform}/tests"

-- ==================
-- Workspace
-- ==================
workspace "OpenAssetTools"
    location "build"
    objdir "%{wks.location}/obj"
    symbols "On"
    systemversion "latest"

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
    filter {}

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "Full"
        flags {
            "FatalWarnings"
        }
    filter {}

-- ========================
-- ThirdParty
-- ========================
include "thirdparty/libtomcrypt.lua"
include "thirdparty/libtommath.lua"
include "thirdparty/salsa20.lua"
include "thirdparty/zlib.lua"

-- ThirdParty group: All projects that are external dependencies
group "ThirdParty"
    libtommath:project()
    libtomcrypt:project()
    salsa20:project()
    zlib:project()
group ""

-- ========================
-- Projects
-- ========================
include "src/Crypto.lua"
include "src/Linker.lua"
include "src/Unlinker.lua"
include "src/Utils.lua"
include "src/ZoneCodeGenerator.lua"
include "src/ZoneCommon.lua"
include "src/ZoneLoading.lua"
include "src/ZoneWriting.lua"

-- Components group: All projects assist or are part of a tool
group "Components"
    Crypto:project()
    Utils:project()
    --ZoneCodeGenerator:project()
    ZoneCommon:project()
    ZoneLoading:project()
    ZoneWriting:project()
group ""

-- Tools group: All projects that compile into the final tools
group "Tools"
    Linker:project()
    Unlinker:project()
group ""

-- ========================
-- Tests
-- ========================
include "test/ZoneCodeGeneratorTests.lua"
include "test/ZoneCommonTests.lua"

-- Tests group: Unit test and other tests projects
group "Tests"
    --ZoneCodeGeneratorTests:project()
    ZoneCommonTests:project()
group ""