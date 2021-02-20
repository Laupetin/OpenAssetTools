ZoneCommonTests = {}

function ZoneCommonTests:include()
    if References:include(self:name()) then
		includedirs {
			path.join(TestFolder(), "ZoneCommonTests")
		}
	end
end

function ZoneCommonTests:link()
	if References:link(self:name()) then
		links(self:name())
	end
end

function ZoneCommonTests:use()
	
end

function ZoneCommonTests:name()
    return "ZoneCommonTests"
end

function ZoneCommonTests:project()
	References:reset()
	local folder = TestFolder();

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
		
		self:include()
		ZoneCommon:include()
		catch2:include()

		ZoneCommon:link()

		ZoneCode:use()
end
