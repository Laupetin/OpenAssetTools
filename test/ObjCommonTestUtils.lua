ObjCommonTestUtils = {}

function ObjCommonTestUtils:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "ObjCommonTestUtils")
		}
	end
end

function ObjCommonTestUtils:link(links)
	links:add(self:name())
	links:linkto(ObjCommon)
end

function ObjCommonTestUtils:use()
	
end

function ObjCommonTestUtils:name()
    return "ObjCommonTestUtils"
end

function ObjCommonTestUtils:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ObjCommonTestUtils/**.h"), 
			path.join(folder, "ObjCommonTestUtils/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjCommonTestUtils")
			}
		}
		
		self:include(includes)
		ObjCommon:include(includes)
		catch2:include(includes)

		links:linkto(ObjCommon)
		links:linkto(catch2)
		links:linkall()
end
