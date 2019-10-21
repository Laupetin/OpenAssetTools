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

-- All projects here should be in the thirdparty folder
group "thirdparty"

libtommath:project()
libtomcrypt:project()
salsa20:project()
zlib:project()

-- Reset group
group ""