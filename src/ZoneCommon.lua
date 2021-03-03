ZoneCommon = {}

function ZoneCommon:include()
	if References:include(self:name()) then
		Utils:include()
		ObjCommon:include()
		includedirs {
			path.join(ProjectFolder(), "ZoneCommon")
		}
	end
end

function ZoneCommon:link()
	if References:link(self:name()) then
		links(self:name())
		Utils:link()
		ObjCommon:link()
	end
end

function ZoneCommon:use()
	
end

function ZoneCommon:name()
	return "ZoneCommon"
end

function ZoneCommon:project()
	References:reset()
	local folder = ProjectFolder();

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneCommon/**.h"), 
			path.join(folder, "ZoneCommon/**.cpp") 
		}
		
        self:include()

		Utils:link()
end
