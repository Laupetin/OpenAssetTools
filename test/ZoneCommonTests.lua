ZoneCommonTests = {}

function ZoneCommonTests:include(includes)
    if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "ZoneCommonTests")
		}
	end
end

function ZoneCommonTests:link(links)
	
end

function ZoneCommonTests:use()
	
end

function ZoneCommonTests:name()
    return "ZoneCommonTests"
end

function ZoneCommonTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ZoneCommonTests/**.h"), 
			path.join(folder, "ZoneCommonTests/**.cpp"),
			ZoneCode:allTestFiles()
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ZoneCommonTests"),
				path.join(BuildFolder(), "src/ZoneCode")
			}
		}
		
		self:include(includes)
		ZoneCommon:include(includes)
		catch2:include(includes)

		links:linkto(ZoneCommon)
		links:linkall()

		ZoneCode:use()
end
