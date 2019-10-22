zlib = {}

function zlib:include()

	defines {
		"ZLIB_CONST"
	}

	includedirs {
		path.join(ThirdPartyFolder(), "zlib")
	}
end

function zlib:link()
	links {
		"zlib"
	}
end

function zlib:project()
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
