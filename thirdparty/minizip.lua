minizip = {}

function minizip:include(includes)
	if includes:handle(self:name()) then
		zlib:include(includes)
		includedirs {
			path.join(ThirdPartyFolder(), "zlib/contrib/minizip")
		}
	end
end

function minizip:link(links)
	links:add(self:name())
	links:linkto(zlib)
end

function minizip:use()
	
end

function minizip:name()
	return "minizip"
end

function minizip:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
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
		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
        warnings "off"
end
