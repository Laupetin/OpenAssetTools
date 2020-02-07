minizip = {}

function minizip:include()
	if References:include("minizip") then
		zlib:include()
		includedirs {
			path.join(ThirdPartyFolder(), "zlib/contrib/minizip")
		}
	end
end

function minizip:link()
	if References:link("minizip") then
		zlib:link()
		links "minizip"
	end
end

function minizip:use()
	
end

function minizip:project()
	References:reset()
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
			path.join(folder, "zlib/contrib/minizip/ioapi.h"),
            path.join(folder, "zlib/contrib/minizip/ioapi.c"),
			path.join(folder, "zlib/contrib/minizip/crypt.h"),
        }
		
		self:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
        warnings "off"
end
