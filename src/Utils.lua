Utils = {}

function Utils:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "Utils")
		}
	end
end

function Utils:link(links)
	links:add(self:name())
end

function Utils:use()
	
end

function Utils:name()
    return "Utils"
end

function Utils:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

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
		
        self:include(includes)
end
