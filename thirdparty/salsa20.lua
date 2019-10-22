salsa20 = {}

function salsa20:include()
	includedirs {
		path.join(ThirdPartyFolder(), "salsa20")
	}
end

function salsa20:link()
	links {
		"salsa20"
	}
end

function salsa20:project()
	local folder = ThirdPartyFolder();

	project "salsa20"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "salsa20/*.h"), 
			path.join(folder, "salsa20/*.c") 
		}
		
		self:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
