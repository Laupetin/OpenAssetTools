libtommath = {}

function libtommath:include()
	includedirs {
		path.join(ThirdPartyFolder(), "libtommath")
	}
end

function libtommath:link()
	links {
		"libtommath"
	}
end

function libtommath:project()
	local folder = ThirdPartyFolder();

	project "libtommath"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "libtommath/*.h"), 
			path.join(folder, "libtommath/*.c") 
		}
		
		self:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
