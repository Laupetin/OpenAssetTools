ObjLoading = {}

function ObjLoading:include()
	ZoneCommon:include()
	includedirs {
		path.join(ProjectFolder(), "ObjLoading")
	}
end

function ObjLoading:link()
    Utils:link()
	ZoneCommon:link()
	links {
		"ObjLoading"
	}
end

function ObjLoading:use()
	
end

function ObjLoading:project()
	local folder = ProjectFolder();

	project "ObjLoading"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ObjLoading/**.h"), 
			path.join(folder, "ObjLoading/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjLoading")
			}
		}
		
        self:include()
		Utils:include()
end
