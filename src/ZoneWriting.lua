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

function ZoneWriting:project()
	local folder = ProjectFolder();

	project "ZoneWriting"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneWriting/**.h"), 
			path.join(folder, "ZoneWriting/**.cpp") 
		}
		
        self:include()
        Crypto:include()
        Utils:include()
		zlib:include()
end
