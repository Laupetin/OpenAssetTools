ObjCompiling = {}

function ObjCompiling:include(includes)
	if includes:handle(self:name()) then
		ObjCommon:include(includes)
		ObjLoading:include(includes)
		ObjImage:include(includes)
		ZoneCommon:include(includes)
		includedirs {
			path.join(ProjectFolder(), "ObjCompiling")
		}
	end
end

function ObjCompiling:link(links)
	links:add(self:name())
	links:linkto(Utils)
	links:linkto(ObjCommon)
	links:linkto(ObjLoading)
	links:linkto(ObjImage)
	links:linkto(ZoneCommon)
end

function ObjCompiling:use()
	
end

function ObjCompiling:name()
	return "ObjCompiling"
end

function ObjCompiling:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ObjCompiling/**.h"), 
			path.join(folder, "ObjCompiling/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjCompiling")
			}
		}
		
		self:include(includes)
		Utils:include(includes)
		json:include(includes)
end
