ParserTestUtils = {}

function ParserTestUtils:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "ParserTestUtils")
		}
	end
end

function ParserTestUtils:link(links)
	links:add(self:name())
	links:linkto(Parser)
end

function ParserTestUtils:use()
	
end

function ParserTestUtils:name()
    return "ParserTestUtils"
end

function ParserTestUtils:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ParserTestUtils/**.h"), 
			path.join(folder, "ParserTestUtils/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ParserTestUtils")
			}
		}
		
		self:include(includes)
		Parser:include(includes)
		catch2:include(includes)

		links:linkto(Parser)
		links:linkall()
end
