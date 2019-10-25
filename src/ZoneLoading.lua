ZoneLoading = {}

function ZoneLoading:include()
	ZoneCommon:include()
	includedirs {
		path.join(ProjectFolder(), "ZoneLoading")
	}
end

function ZoneLoading:link()
    Crypto:link()
    Utils:link()
	ZoneCommon:link()
	zlib:link()
	links {
		"ZoneLoading"
	}
end

function ZoneLoading:use()
	
end

function ZoneLoading:project()
	local folder = ProjectFolder();

	project "ZoneLoading"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneLoading/**.h"), 
			path.join(folder, "ZoneLoading/**.cpp"),
			ZoneCode:allLoadFiles()
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ZoneLoading"),
				path.join(BuildFolder(), "src/ZoneCode")
			}
		}
		
        self:include()
        Crypto:include()
		Utils:include()
		zlib:include()
		ZoneCode:include()

		ZoneCode:use()
end
