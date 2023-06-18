ObjCommonTests = {}

function ObjCommonTests:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "ObjCommonTests")
		}
	end
end

function ObjCommonTests:link(links)
	
end

function ObjCommonTests:use()
	
end

function ObjCommonTests:name()
    return "ObjCommonTests"
end

function ObjCommonTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ObjCommonTests/**.h"), 
			path.join(folder, "ObjCommonTests/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjCommonTests"),
				path.join(BuildFolder(), "src/ZoneCode")
			}
		}
		
		self:include(includes)
		ObjCommon:include(includes)
		catch2:include(includes)

		links:linkto(ObjCommon)
		links:linkto(catch2)
		links:linkall()
end
