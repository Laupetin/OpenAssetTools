ZoneWriting = {}

function ZoneWriting:include()
	ZoneCommon:include()
	includedirs {
		path.join(ProjectFolder(), "ZoneWriting")
	}
end

function ZoneWriting:link()
    Crypto:link()
    Utils:link()
    ZoneCommon:link()
	zlib:link()
	links {
		"ZoneWriting"
	}
end

function ZoneWriting:use()
	
end

function ZoneWriting:project()
	local folder = ProjectFolder();

	project "ZoneWriting"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneWriting/**.h"), 
			path.join(folder, "ZoneWriting/**.cpp") 
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ZoneWriting")
			}
		}
		
        self:include()
        Crypto:include()
        Utils:include()
		zlib:include()

		ZoneCode:use()
end
