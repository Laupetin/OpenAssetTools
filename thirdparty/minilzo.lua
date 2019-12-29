minilzo = {}

function minilzo:include()
	includedirs {
		path.join(ThirdPartyFolder(), "minilzo")
	}
end

function minilzo:link()
	links "minilzo"
end

function minilzo:use()
	
end

function minilzo:project()
	local folder = ThirdPartyFolder();

	project "minilzo"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "minilzo/*.h"), 
			path.join(folder, "minilzo/*.c") 
		}
		
		self:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
