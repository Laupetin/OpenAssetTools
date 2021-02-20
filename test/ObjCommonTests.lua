ObjCommonTests = {}

function ObjCommonTests:include()
	if References:include(self:name()) then
		includedirs {
			path.join(TestFolder(), "ObjCommonTests")
		}
	end
end

function ObjCommonTests:link()
	if References:link(self:name()) then
		links(self:name())
	end
end

function ObjCommonTests:use()
	
end

function ObjCommonTests:name()
    return "ObjCommonTests"
end

function ObjCommonTests:project()
	References:reset()
	local folder = TestFolder();

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
		
		self:include()
		ObjCommon:include()
		catch2:include()

		ObjCommon:link()
end
