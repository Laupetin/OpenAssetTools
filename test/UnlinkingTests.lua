UnlinkingTests = {}

function UnlinkingTests:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			"%{wks.location}/src/Unlinking",
			path.join(TestFolder(), "UnlinkingTests")
		}
	end
end

function UnlinkingTests:link(links)
	
end

function UnlinkingTests:use()
	
end

function UnlinkingTests:name()
    return "UnlinkingTests"
end

function UnlinkingTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "UnlinkingTests/**.h"), 
			path.join(folder, "UnlinkingTests/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "UnlinkingTests")
			}
		}
		
		self:include(includes)
		Catch2Common:include(includes)
		ObjCommonTestUtils:include(includes)
		Unlinking:include(includes)
		catch2:include(includes)

		links:linkto(ObjCommonTestUtils)
		links:linkto(ParserTestUtils)
		links:linkto(Unlinking)
		links:linkto(catch2)
		links:linkto(Catch2Common)
		links:linkall()
end
