ufbx = {}

function ufbx:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "ufbx")
		}
	end
end

function ufbx:link(links)
	links:add(self:name())
end

function ufbx:use()
	
end

function ufbx:name()
    return "ufbx"
end

function ufbx:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files { 
			path.join(folder, "ufbx/*.h"), 
			path.join(folder, "ufbx/*.c") 
		}
		
		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end
