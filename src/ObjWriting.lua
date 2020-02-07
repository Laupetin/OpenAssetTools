ObjWriting = {}

function ObjWriting:include()
	if References:include("ObjWriting") then
		ObjCommon:include()
		ZoneCommon:include()
		includedirs {
			path.join(ProjectFolder(), "ObjWriting")
		}
	end
end

function ObjWriting:link()
	if References:link("ObjWriting") then
		Utils:link()
		ObjCommon:link()
		ZoneCommon:link()
		minilzo:link()
		minizip:link()
		links {
			"ObjWriting"
		}
	end
end

function ObjWriting:use()
	
end

function ObjWriting:project()
	References:reset()
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
		minilzo:include()
		minizip:include()
end
