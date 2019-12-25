ObjWriting = {}

function ObjWriting:include()
	ZoneCommon:include()
	includedirs {
		path.join(ProjectFolder(), "ObjWriting")
	}
end

function ObjWriting:link()
    Utils:link()
	ZoneCommon:link()
	links {
		"ObjWriting"
	}
end

function ObjWriting:use()
	
end

function ObjWriting:project()
	local folder = ProjectFolder();

	project "ObjWriting"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ObjWriting/**.h"), 
			path.join(folder, "ObjWriting/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjWriting")
			}
		}
		
        self:include()
		Utils:include()
end
