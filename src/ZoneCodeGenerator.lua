ZoneCodeGenerator = {}

function ZoneCodeGenerator:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "ZoneCodeGenerator")
		}
		Utils:include(includes)
	end
end

function ZoneCodeGenerator:link(links)

end

function ZoneCodeGenerator:use()
    dependson(self:name())
end

function ZoneCodeGenerator:name()
    return "ZoneCodeGenerator"
end

function ZoneCodeGenerator:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
		targetdir(TargetDirectoryBuildTools)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ZoneCodeGenerator/**.h"), 
			path.join(folder, "ZoneCodeGenerator/**.cpp") 
		}
        vpaths {
			["*"] = {
				path.join(folder, "ZoneCodeGenerator"),
			}
		}
		
		self:include(includes)
		ZoneCodeGeneratorLib:include(includes)

		links:linkto(Utils)
		links:linkto(ZoneCodeGeneratorLib)
		links:linkall()
end
