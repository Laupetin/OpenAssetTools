lzx = {}

function lzx:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "lzx")
		}
	end
end

function lzx:link(links)
	links:add(self:name())
end

function lzx:use()
	
end

function lzx:name()
	return "lzx"
end

function lzx:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "lzx/*.h"),
			path.join(folder, "lzx/*.c")
		}
		
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_DEPRECATE"
		}

		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
