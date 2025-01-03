ObjCompilingTests = {}

function ObjCompilingTests:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "ObjCompilingTests")
		}
	end
end

function ObjCompilingTests:link(links)
	
end

function ObjCompilingTests:use()
	
end

function ObjCompilingTests:name()
    return "ObjCompilingTests"
end

function ObjCompilingTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ObjCompilingTests/**.h"), 
			path.join(folder, "ObjCompilingTests/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjCompilingTests")
			}
		}
		
		self:include(includes)
		ObjCommonTestUtils:include(includes)
		ParserTestUtils:include(includes)
		ObjLoading:include(includes)
		ObjCompiling:include(includes)
		catch2:include(includes)

		links:linkto(ObjCommonTestUtils)
		links:linkto(ParserTestUtils)
		links:linkto(ObjLoading)
		links:linkto(ObjCompiling)
		links:linkto(catch2)
		links:linkall()
end
