ZoneCodeGeneratorLibTests = {}

function ZoneCodeGeneratorLibTests:include()
	if References:include(self:name()) then
		includedirs {
			path.join(TestFolder(), "ZoneCodeGeneratorLibTests")
		}
	end
end

function ZoneCodeGeneratorLibTests:link()
	if References:link(self:name()) then
		links(self:name())
	end
end

function ZoneCodeGeneratorLibTests:use()
	
end

function ZoneCodeGeneratorLibTests:name()
    return "ZoneCodeGeneratorLibTests"
end

function ZoneCodeGeneratorLibTests:project()
	References:reset()
	local folder = TestFolder();

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ZoneCodeGeneratorLibTests/**.h"), 
			path.join(folder, "ZoneCodeGeneratorLibTests/**.cpp")
		}

        vpaths {
			["*"] = {
				path.join(folder, "ZoneCodeGeneratorLibTests"),
			}
		}
		
		self:include()
		ZoneCodeGeneratorLib:include()
		catch2:include()

		ZoneCodeGeneratorLib:link()
end
