ZoneCodeGeneratorTests = {}

function ZoneCodeGeneratorTests:include()
    
end

function ZoneCodeGeneratorTests:link()
	if References:link("ZoneCodeGeneratorTests") then
		links "ZoneCodeGeneratorTests"
	end
end

function ZoneCodeGeneratorTests:use()
	
end

function ZoneCodeGeneratorTests:project()
	References:reset()
	local folder = TestFolder();

	project "ZoneCodeGeneratorTests"
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "SharedLib"
		language "C#"
        dotnetframework "4.5"
		
		files {
			path.join(folder, "ZoneCodeGeneratorTests/**.cs")
		}

        vpaths { ["*"] = "test/ZoneCodeGeneratorTests" }

        nuget {
            "Moq:4.13.1",
			"MSTest.TestFramework:2.0.0",
			"MSTest.TestAdapter:2.0.0"
        }
        
        links {
            "System",
            "System.Core",
			"System.Data"
		}
		
		ZoneCodeGenerator:link()
end
