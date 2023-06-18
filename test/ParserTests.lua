ParserTests = {}

function ParserTests:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "ParserTests")
		}
	end
end

function ParserTests:link(links)
	
end

function ParserTests:use()
	
end

function ParserTests:name()
    return "ParserTests"
end

function ParserTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ParserTests/**.h"), 
			path.join(folder, "ParserTests/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ParserTests")
			}
		}
		
		self:include(includes)
		ParserTestUtils:include(includes)
		Parser:include(includes)
		catch2:include(includes)

		links:linkto(ParserTestUtils)
		links:linkto(Parser)
		links:linkto(catch2)
		links:linkall()
end
