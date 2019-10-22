ZoneCommonTests = {}

function ZoneCommonTests:include()
    
end

function ZoneCommonTests:link()
    
end

function ZoneCommonTests:project()
	local folder = TestFolder();

	project "ZoneCommonTests"
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test"
		kind "SharedLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneCommonTests/**.h"), 
			path.join(folder, "ZoneCommonTests/**.cpp") 
		}
		
		self:include()
		ZoneCommon:include()

		ZoneCommon:link()

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
