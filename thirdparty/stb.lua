stb = {}

function stb:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "stb")
		}
	end
end

function stb:link(links)
	links:add(self:name())
end

function stb:use()
	
end

function stb:name()
    return "stb"
end

function stb:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files { 
			path.join(folder, "stb/*.h"),
			path.join(folder, "stb_impl/*.cpp")
		}
		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
