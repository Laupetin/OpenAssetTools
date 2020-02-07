libtommath = {}

function libtommath:include()
	if References:include("libtommath") then
		includedirs {
			path.join(ThirdPartyFolder(), "libtommath")
		}
	end
end

function libtommath:link()
	if References:link("libtommath") then
		links "libtommath"
	end
end

function libtommath:use()
	
end

function libtommath:project()
	References:reset()
	local folder = ThirdPartyFolder();

	project "libtommath"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
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
