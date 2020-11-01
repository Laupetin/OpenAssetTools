ZoneCodeGeneratorNew = {}

function ZoneCodeGeneratorNew:include()
	if References:include(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "ZoneCodeGeneratorNew")
		}
	end
end

function ZoneCodeGeneratorNew:link()
    
end

function ZoneCodeGeneratorNew:use()
    dependson(self:name())
end

function ZoneCodeGeneratorNew:name()
    return "ZoneCodeGeneratorNew"
end

function ZoneCodeGeneratorNew:project()
	References:reset()
	local folder = ProjectFolder();

	project(self:name())
		targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ZoneCodeGeneratorNew/**.h"), 
			path.join(folder, "ZoneCodeGeneratorNew/**.cpp") 
		}
		
		self:include()
		Utils:include()

		Utils:link()
end
