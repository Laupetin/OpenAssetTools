libtomcrypt = {}

function libtomcrypt:include(includes)
	if includes:handle(self:name()) then
		defines{
			"LTM_DESC"
		}

		includedirs {
			path.join(ThirdPartyFolder(), "libtomcrypt/src/headers")
		}
	end
end

function libtomcrypt:link(links)
	links:add(self:name())
	links:linkto(libtommath)
end

function libtomcrypt:use()
	
end

function libtomcrypt:name()
	return "libtomcrypt"
end

function libtomcrypt:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
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
		
		self:include(includes)
		libtommath:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
