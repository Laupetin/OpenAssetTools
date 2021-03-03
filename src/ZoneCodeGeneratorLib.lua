ZoneCodeGeneratorLib = {}

function ZoneCodeGeneratorLib:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "ZoneCodeGeneratorLib")
		}
		Utils:include(includes)
	end
end

function ZoneCodeGeneratorLib:link(links)
	links:add(self:name())
	links:linkto(Utils)
end

function ZoneCodeGeneratorLib:use()
    dependson(self:name())
end

function ZoneCodeGeneratorLib:name()
    return "ZoneCodeGeneratorLib"
end

function ZoneCodeGeneratorLib:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
		targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneCodeGeneratorLib/**.h"), 
			path.join(folder, "ZoneCodeGeneratorLib/**.cpp") 
		}
		
		self:include(includes)
end
