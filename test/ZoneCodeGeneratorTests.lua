ZoneCodeGeneratorTests = {}

function ZoneCodeGeneratorTests:include()
    
end

function ZoneCodeGeneratorTests:link()
    
end

function ZoneCodeGeneratorTests:project()
	local folder = TestFolder();

	project "ZoneCodeGeneratorTests"
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test"
		kind "SharedLib"
		language "C#"
		
		files {
			path.join(folder, "Zone/src/**.h"), 
			path.join(folder, "libtomcrypt/src/**.c") 
		}
		
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_DEPRECATE",
			"LTC_SOURCE",
			"LTC_NO_TEST",
			"LTC_NO_PROTOTYPES"
		}
		
		self:include()
		libtommath:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
