flac = {}

function flac:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "flac")
		}
	end
end

function flac:link(links)
	links:add(self:name())
end

function flac:use()
	
end

function flac:name()
	return "flac"
end

function flac:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "flac/foxen/*.h"),
			path.join(folder, "flac/foxen/*.c")
		}
		
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_DEPRECATE"
		}
		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
