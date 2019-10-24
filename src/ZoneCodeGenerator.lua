ZoneCodeGenerator = {}

function ZoneCodeGenerator:include()

end

function ZoneCodeGenerator:link()
	links {
		"ZoneCodeGenerator"
	}
end

function ZoneCodeGenerator:project()
	local folder = ProjectFolder();

	project "ZoneCodeGenerator"
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
        language "C#"
        namespace "ZoneCodeGenerator"
        
		files {
			path.join(folder, "ZoneCodeGenerator/**.cs")
        }

        vpaths { ["*"] = "src/ZoneCodeGenerator" }

        nuget {
            "Antlr3.Runtime:3.5.1",
            "StringTemplate4:4.0.8"
        }
        
        links {
            "System",
            "System.Core",
            "System.Data"
        }
end
