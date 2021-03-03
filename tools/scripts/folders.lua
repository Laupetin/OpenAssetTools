-- Functions for locating commonly used folders
local _BuildFolder = path.getabsolute("../../build")
function BuildFolder()
	return path.getrelative(os.getcwd(), _BuildFolder)
end

local _ThirdPartyFolder = path.getabsolute("../../thirdparty")
function ThirdPartyFolder()
	return path.getrelative(os.getcwd(), _ThirdPartyFolder)
end

local _ProjectFolder = path.getabsolute("../../src")
function ProjectFolder()
	return path.getrelative(os.getcwd(), _ProjectFolder)
end

local _TestFolder = path.getabsolute("../../test")
function TestFolder()
	return path.getrelative(os.getcwd(), _TestFolder)
end

-- Target Directories
TargetDirectoryBin = "%{wks.location}/bin/%{cfg.buildcfg}_%{cfg.platform}"
TargetDirectoryLib = "%{wks.location}/lib/%{cfg.buildcfg}_%{cfg.platform}"
TargetDirectoryTest = "%{wks.location}/lib/%{cfg.buildcfg}_%{cfg.platform}/tests"