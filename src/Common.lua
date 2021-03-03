Common = {}

function Common:include(includes)
	if includes:handle(self:name()) then
		Utils:include(includes)
		includedirs {
			path.join(ProjectFolder(), "Common")
		}
	end
end

function Common:link(links)
	links:add(self:name())
	links:linkto(Utils)
end

function Common:use()
	
end

function Common:name()
	return "Common"
end

function Common:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "Common/**.h"), 
			path.join(folder, "Common/**.cpp") 
		}
		
        self:include(includes)
end
