Utils = {}

function Utils:include()
	if References:include(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "Utils")
		}
	end
end

function Utils:link()
	if References:link(self:name()) then
		links(self:name())
	end
end

function Utils:use()
	
end

function Utils:name()
    return "Utils"
end

function Utils:project()
	References:reset()
	local folder = ProjectFolder();

	project(self:name())
		targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "Utils/**.h"), 
			path.join(folder, "Utils/**.cpp") 
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "Utils")
			}
		}
		
        self:include()
end
