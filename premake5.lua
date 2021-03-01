-- Functions for locating commonly used folders
local _BuildFolder = path.getabsolute("build")
function BuildFolder()
	return path.getrelative(os.getcwd(), _BuildFolder)
end

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

-- Functions for including projects
References = {
    includeList = {},
    linkList = {}
}

function References:include(name)
    result = self.includeList[name] == nil

    if result then
        self.includeList[name] = true
    end

    return result
end

function References:link(name)
    result = self.linkList[name] == nil

    if result then
        self.linkList[name] = true
    end

    return result
end

function References:reset()
    self.includeList = {}
    self.linkList = {}
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
    cppdialect "C++17"

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
include "thirdparty/catch2.lua"
include "thirdparty/libtomcrypt.lua"
include "thirdparty/libtommath.lua"
include "thirdparty/minilzo.lua"
include "thirdparty/minizip.lua"
include "thirdparty/salsa20.lua"
include "thirdparty/zlib.lua"

-- ThirdParty group: All projects that are external dependencies
group "ThirdParty"
    catch2:project()
    libtommath:project()
    libtomcrypt:project()
    minilzo:project()
    minizip:project()
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
include "src/ZoneCode.lua"
include "src/ZoneCodeGeneratorLib.lua"
include "src/ZoneCodeGenerator.lua"
include "src/ZoneCommon.lua"
include "src/ZoneLoading.lua"
include "src/ZoneWriting.lua"
include "src/ZoneCommon.lua"
include "src/ObjCommon.lua"
include "src/ObjLoading.lua"
include "src/ObjWriting.lua"

-- Components group: All projects assist or are part of a tool
group "Components"
    Crypto:project()
    Utils:project()
    ZoneCode:project()
    ZoneCodeGeneratorLib:project()
    ZoneCommon:project()
    ZoneLoading:project()
    ZoneWriting:project()
    ObjCommon:project()
    ObjLoading:project()
    ObjWriting:project()
group ""

-- Tools group: All projects that compile into the final tools
group "Tools"
    Linker:project()
    Unlinker:project()
    ZoneCodeGenerator:project()
group ""

-- ========================
-- Tests
-- ========================
include "test/ObjCommonTests.lua"
include "test/ZoneCodeGeneratorLibTests.lua"
include "test/ZoneCommonTests.lua"

-- Tests group: Unit test and other tests projects
group "Tests"
    ObjCommonTests:project()
    ZoneCodeGeneratorLibTests:project()
    ZoneCommonTests:project()
group ""