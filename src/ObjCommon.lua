ObjCommon = {}

function ObjCommon:include()
	ZoneCommon:include()
	minizip:include()
	includedirs {
		path.join(ProjectFolder(), "ObjCommon")
	}
end

function ObjCommon:link()
    Utils:link()
	ZoneCommon:link()
	minizip:link()
	links {
		"ObjCommon"
	}
end

function ObjCommon:use()
	
end

function ObjCommon:project()
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
