zlib = {}

function zlib:include()
	if References:include("zlib") then
		defines {
			"ZLIB_CONST"
		}

		includedirs {
			path.join(ThirdPartyFolder(), "zlib")
		}
	end
end

function zlib:link()
	if References:link("zlib") then
		links "zlib"
	end
end

function zlib:use()
	
end

function zlib:project()
	References:reset()
	local folder = ThirdPartyFolder();

	project "zlib"
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
		
		self:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
