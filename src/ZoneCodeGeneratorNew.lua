ZoneCodeGeneratorNew = {}

function ZoneCodeGeneratorNew:include()
	if References:include(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "ZoneCodeGeneratorNew")
		}
	end
	Utils:include()
end

function ZoneCodeGeneratorNew:link()
	if References:link(self:name()) then
		links(self:name())
	end
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
		targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ZoneCodeGeneratorNew/**.h"), 
			path.join(folder, "ZoneCodeGeneratorNew/**.cpp") 
		}
        vpaths {
			["*"] = {
				path.join(folder, "ZoneCodeGeneratorNew"),
			}
		}
		
		self:include()
		ZoneCodeGeneratorLib:include()

		Utils:link()
		ZoneCodeGeneratorLib:link()
end
