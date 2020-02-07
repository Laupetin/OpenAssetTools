ZoneCommon = {}

function ZoneCommon:include()
	if References:include("ZoneCommon") then
		Utils:include()
		ObjCommon:include()
		includedirs {
			path.join(ProjectFolder(), "ZoneCommon")
		}
	end
end

function ZoneCommon:link()
	if References:link("ZoneCommon") then
		Utils:link()
		ObjCommon:link()
		links {
			"ZoneCommon"
		}
	end
end

function ZoneCommon:use()
	
end

function ZoneCommon:project()
	References:reset()
	local folder = ProjectFolder();

	project "ZoneCommon"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneCommon/**.h"), 
			path.join(folder, "ZoneCommon/**.cpp") 
		}
		
        self:include()
end
