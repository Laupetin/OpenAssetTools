ObjCommon = {}

function ObjCommon:include()
	if References:include("ObjCommon") then
		ZoneCommon:include()
		minizip:include()
		includedirs {
			path.join(ProjectFolder(), "ObjCommon")
		}
	end
end

function ObjCommon:link()
	if References:link("ObjCommon") then
		Utils:link()
		ZoneCommon:link()
		minizip:link()
		links {
			"ObjCommon"
		}
	end
end

function ObjCommon:use()
	
end

function ObjCommon:project()
	References:reset()
	local folder = ProjectFolder();

	project "ObjCommon"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ObjCommon/**.h"), 
			path.join(folder, "ObjCommon/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjCommon")
			}
		}
		
        self:include()
		Utils:include()
end
