Unlinker = {}

function Unlinker:include()
	includedirs {
		path.join(ProjectFolder(), "Unlinker")
	}
end

function Unlinker:link()
    
end

function Unlinker:use()
    dependson "Unlinker"
end

function Unlinker:project()
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
        ObjWriting:include()

		Utils:link()
        ZoneLoading:link()
        ObjWriting:link()
end
