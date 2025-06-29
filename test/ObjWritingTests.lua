ObjWritingTests = {}

function ObjWritingTests:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "ObjWritingTests")
		}
	end
end

function ObjWritingTests:link(links)
	
end

function ObjWritingTests:use()
	
end

function ObjWritingTests:name()
    return "ObjWritingTests"
end

function ObjWritingTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ObjWritingTests/**.h"), 
			path.join(folder, "ObjWritingTests/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjWritingTests")
			}
		}
		
		self:include(includes)
		Catch2Common:include(includes)
		ObjCommonTestUtils:include(includes)
		ParserTestUtils:include(includes)
		ObjWriting:include(includes)
		catch2:include(includes)

		links:linkto(ObjCommonTestUtils)
		links:linkto(ParserTestUtils)
		links:linkto(ObjWriting)
		links:linkto(catch2)
		links:linkto(Catch2Common)
		links:linkall()
end
