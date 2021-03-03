ZoneCodeGeneratorLib = {}

function ZoneCodeGeneratorLib:include()
	if References:include(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "ZoneCodeGeneratorLib")
		}
	end
	Utils:include()
end

function ZoneCodeGeneratorLib:link()
	if References:link(self:name()) then
		links(self:name())
		Utils:link()
	end
end

function ZoneCodeGeneratorLib:use()
    dependson(self:name())
end

function ZoneCodeGeneratorLib:name()
    return "ZoneCodeGeneratorLib"
end

function ZoneCodeGeneratorLib:project()
	References:reset()
	local folder = ProjectFolder();

	project(self:name())
		targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneCodeGeneratorLib/**.h"), 
			path.join(folder, "ZoneCodeGeneratorLib/**.cpp") 
		}
		
		self:include()

		Utils:link()
end
