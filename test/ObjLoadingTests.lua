ObjLoadingTests = {}

function ObjLoadingTests:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "ObjLoadingTests")
		}
	end
end

function ObjLoadingTests:link(links)
	
end

function ObjLoadingTests:use()
	
end

function ObjLoadingTests:name()
    return "ObjLoadingTests"
end

function ObjLoadingTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ObjLoadingTests/**.h"), 
			path.join(folder, "ObjLoadingTests/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjLoadingTests")
			}
		}
		
		self:include(includes)
		Catch2Common:include(includes)
		ObjCommonTestUtils:include(includes)
		ParserTestUtils:include(includes)
		ObjLoading:include(includes)
		catch2:include(includes)

		links:linkto(ObjCommonTestUtils)
		links:linkto(ParserTestUtils)
		links:linkto(ObjLoading)
		links:linkto(catch2)
		links:linkto(Catch2Common)
		links:linkall()
end
