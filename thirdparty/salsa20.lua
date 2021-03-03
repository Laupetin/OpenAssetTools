salsa20 = {}

function salsa20:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "salsa20")
		}
	end
end

function salsa20:link(links)
	links:add(self:name())
end

function salsa20:use()
	
end

function salsa20:name()
	return "salsa20"
end

function salsa20:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "salsa20/*.h"), 
			path.join(folder, "salsa20/*.c") 
		}
		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
