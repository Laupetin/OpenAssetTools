ZoneCodeGeneratorLibTests = {}

function ZoneCodeGeneratorLibTests:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "ZoneCodeGeneratorLibTests")
		}
	end
end

function ZoneCodeGeneratorLibTests:link(links)
	
end

function ZoneCodeGeneratorLibTests:use()
	
end

function ZoneCodeGeneratorLibTests:name()
    return "ZoneCodeGeneratorLibTests"
end

function ZoneCodeGeneratorLibTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

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
		
		self:include(includes)
		ZoneCodeGeneratorLib:include(includes)
		catch2:include(includes)

		links:linkto(ZoneCodeGeneratorLib)
		links:linkall()
end
