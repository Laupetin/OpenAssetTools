minilzo = {}

function minilzo:include()
	if References:include("minilzo") then
		includedirs {
			path.join(ThirdPartyFolder(), "minilzo")
		}
	end
end

function minilzo:link()
	if References:link("minilzo") then
		links "minilzo"
	end
end

function minilzo:use()
	
end

function minilzo:project()
	References:reset()
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
