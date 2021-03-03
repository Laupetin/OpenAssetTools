minilzo = {}

function minilzo:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "minilzo")
		}
	end
end

function minilzo:link(links)
	links:add(self:name())
end

function minilzo:use()
	
end

function minilzo:name()
	return "minilzo"
end

function minilzo:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C"
		
		files { 
			path.join(folder, "minilzo/*.h"), 
			path.join(folder, "minilzo/*.c") 
		}
		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
