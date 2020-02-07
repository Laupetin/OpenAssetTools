ZoneCodeGenerator = {}

function ZoneCodeGenerator:include()

end

function ZoneCodeGenerator:link()
	if References:link("ZoneCodeGenerator") then
        links "ZoneCodeGenerator"
    end
end

function ZoneCodeGenerator:use()
    dependson "ZoneCodeGenerator"
end

function ZoneCodeGenerator:project()
    References:reset()
	local folder = ProjectFolder();

	project "ZoneCodeGenerator"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
        language "C#"
        dotnetframework "4.5"
        namespace "ZoneCodeGenerator"
        
		files {
            path.join(folder, "ZoneCodeGenerator/**.cs"),
            path.join(folder, "ZoneCodeGenerator/**.stg")
        }

        filter "files:**.stg"
            buildaction "Embed"
        filter {}

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
