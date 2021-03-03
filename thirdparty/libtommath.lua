libtommath = {}

function libtommath:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "libtommath")
		}
	end
end

function libtommath:link(links)
	links:add(self:name())
end

function libtommath:use()
	
end

function libtommath:name()
	return "libtommath"
end

function libtommath:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "libtommath/*.h"), 
			path.join(folder, "libtommath/*.c") 
		}
		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
