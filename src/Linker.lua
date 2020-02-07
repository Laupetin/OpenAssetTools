Linker = {}

function Linker:include()
	if References:include("Linker") then
		includedirs {
			path.join(ProjectFolder(), "Linker")
		}
	end
end

function Linker:link()
    
end

function Linker:use()
    dependson "Linker"
end

function Linker:project()
	References:reset()
	local folder = ProjectFolder();

	project "Linker"
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "Linker/**.h"), 
			path.join(folder, "Linker/**.cpp") 
		}
		
		self:include()
		Utils:include()
        ZoneWriting:include()

		Utils:link()
        --ZoneWriting:link()
end
