ZoneCommon = {}

function ZoneCommon:include()
	includedirs {
		path.join(ProjectFolder(), "ZoneCommon")
	}
end

function ZoneCommon:link()
	links {
		"ZoneCommon"
	}
end

function ZoneCommon:project()
	local folder = ProjectFolder();

	project "ZoneCommon"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneCommon/**.h"), 
			path.join(folder, "ZoneCommon/**.cpp") 
		}
		
        self:include()
end
