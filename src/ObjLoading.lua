ObjLoading = {}

function ObjLoading:include()
	if References:include("ObjLoading") then
		ObjCommon:include()
		ZoneCommon:include()
		includedirs {
			path.join(ProjectFolder(), "ObjLoading")
		}
	end
end

function ObjLoading:link()
	if References:link("ObjLoading") then
		Utils:link()
		ObjCommon:link()
		ZoneCommon:link()
		minilzo:link()
		minizip:link()
		zlib:link()
		links {
			"ObjLoading"
		}
	end
end

function ObjLoading:use()
	
end

function ObjLoading:project()
	References:reset()
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
		Crypto:include()
		Utils:include()
		minilzo:include()
		minizip:include()
		zlib:include()
end
