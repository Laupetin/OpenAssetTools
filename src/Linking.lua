Linking = {}

function Linking:include(includes)
	if includes:handle(self:name()) then
		ZoneCommon:include(includes)
		includedirs {
			path.join(ProjectFolder(), "Linking")
		}
	end
end

function Linking:link(links)
	links:add(self:name())
	links:linkto(Utils)
	links:linkto(ObjCompiling)
	links:linkto(ZoneLoading)
	links:linkto(ZoneWriting)
	links:linkto(ObjLoading)
	links:linkto(ObjWriting)
end

function Linking:use()
	
end

function Linking:name()
	return "Linking"
end

function Linking:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "Linking/**.h"), 
			path.join(folder, "Linking/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "Linking")
			}
		}

		ObjCommon:use()
		useSourceTemplating("Linking")
		
		self:include(includes)
		Utils:include(includes)
        ZoneLoading:include(includes)
        ObjCompiling:include(includes)
        ObjLoading:include(includes)
        ObjWriting:include(includes)
        ZoneWriting:include(includes)
end
