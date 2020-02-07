libtomcrypt = {}

function libtomcrypt:include()
	if References:include("libtomcrypt") then
		defines{
			"LTM_DESC"
		}

		includedirs {
			path.join(ThirdPartyFolder(), "libtomcrypt/src/headers")
		}
	end
end

function libtomcrypt:link()
	if References:link("libtomcrypt") then
		links "libtomcrypt"
	end
end

function libtomcrypt:use()
	
end

function libtomcrypt:project()
	References:reset()
	local folder = ThirdPartyFolder();

	project "libtomcrypt"
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files {
			path.join(folder, "libtomcrypt/src/**.h"), 
			path.join(folder, "libtomcrypt/src/**.c") 
		}
		
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_DEPRECATE",
			"LTC_SOURCE",
			"LTC_NO_TEST",
			"LTC_NO_PROTOTYPES"
		}
		
		self:include()
		libtommath:include()

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
