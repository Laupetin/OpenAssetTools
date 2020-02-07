Unlinker = {}

function Unlinker:include()
	if References:include("Unlinker") then
		includedirs {
			path.join(ProjectFolder(), "Unlinker")
		}
	end
end

function Unlinker:link()
    
end

function Unlinker:use()
    dependson "Unlinker"
end

function Unlinker:project()
	References:reset()
	local folder = ProjectFolder();

	project "Unlinker"
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "Unlinker/**.h"), 
			path.join(folder, "Unlinker/**.cpp") 
		}
		
		self:include()
		Utils:include()
        ZoneLoading:include()
        ObjLoading:include()
        ObjWriting:include()

		Utils:link()
        ZoneLoading:link()
        ObjLoading:link()
        ObjWriting:link()
end
