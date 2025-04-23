lz4 = {}

function lz4:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "lz4/lib")
		}
	end
end

function lz4:link(links)
	links:add(self:name())
end

function lz4:use()
	
end

function lz4:name()
	return "lz4"
end

function lz4:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "lz4/lib/*.h"),
			path.join(folder, "lz4/lib/*.c")
		}
		
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_DEPRECATE"
		}

		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
