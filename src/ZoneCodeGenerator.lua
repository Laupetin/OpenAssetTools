ZoneCodeGenerator = {}

function ZoneCodeGenerator:include()
	if References:include(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "ZoneCodeGenerator")
		}
	end
	Utils:include()
end

function ZoneCodeGenerator:link()
	if References:link(self:name()) then
		links(self:name())
	end
end

function ZoneCodeGenerator:use()
    dependson(self:name())
end

function ZoneCodeGenerator:name()
    return "ZoneCodeGenerator"
end

function ZoneCodeGenerator:project()
	References:reset()
	local folder = ProjectFolder();

	project(self:name())
		targetdir(TargetDirectoryBin)
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
		
		self:include()
		ZoneCodeGeneratorLib:include()

		Utils:link()
		ZoneCodeGeneratorLib:link()
end
