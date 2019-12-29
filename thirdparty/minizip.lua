minizip = {}

function minizip:include()
    zlib:include()
	includedirs {
		path.join(ThirdPartyFolder(), "zlib/contrib/minizip")
	}
end

function minizip:link()
    zlib:link()
	links "minizip"
end

function minizip:use()
	
end

function minizip:project()
	local folder = ThirdPartyFolder();

	project "minizip"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "zlib/contrib/minizip/unzip.h"),
			path.join(folder, "zlib/contrib/minizip/unzip.c"),
			path.join(folder, "zlib/contrib/minizip/zip.h"),
            path.join(folder, "zlib/contrib/minizip/zip.c"),
        }
		
		self:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
        warnings "off"
end
