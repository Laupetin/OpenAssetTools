zlib = {}

function zlib:include(includes)
	if includes:handle(self:name()) then
		defines {
			"ZLIB_CONST"
		}

		includedirs {
			path.join(ThirdPartyFolder(), "zlib")
		}
	end
end

function zlib:link(links)
	links:add(self:name())
end

function zlib:use()
	
end

function zlib:name()
	return "zlib"
end

function zlib:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "zlib/*.h"),
			path.join(folder, "zlib/*.c")
		}
		
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_DEPRECATE"
		}
		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
