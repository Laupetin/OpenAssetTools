Parser = {}

function Parser:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "Parser")
		}
        Utils:include(includes)
	end
end

function Parser:link(links)
	links:add(self:name())
    links:linkto(Utils)
end

function Parser:use()
	
end

function Parser:name()
	return "Parser"
end

function Parser:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "Parser/**.h"), 
			path.join(folder, "Parser/**.cpp") 
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "Parser")
			}
		}
		
		self:include(includes)
end
