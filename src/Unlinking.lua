Unlinking = {}

function Unlinking:include(includes)
	if includes:handle(self:name()) then
		ZoneCommon:include(includes)
		includedirs {
			path.join(ProjectFolder(), "Unlinking"),
		}
	end
end

function Unlinking:link(links)
	links:add(self:name())
	links:linkto(Utils)
	links:linkto(ZoneLoading)
	links:linkto(ObjLoading)
	links:linkto(ObjWriting)
end

function Unlinking:use()
	
end

function Unlinking:name()
	return "Unlinking"
end

function Unlinking:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "Unlinking/**.h"), 
			path.join(folder, "Unlinking/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "Unlinking")
			}
		}

		ObjCommon:use()
		useSourceTemplating("Unlinking")
		
		self:include(includes)
		Utils:include(includes)
        ZoneLoading:include(includes)
        ObjLoading:include(includes)
        ObjWriting:include(includes)
end
