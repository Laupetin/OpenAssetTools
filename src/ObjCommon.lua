ObjCommon = {}

function ObjCommon:include(includes)
	if includes:handle(self:name()) then
		Common:include(includes)
		minizip:include(includes)
		includedirs {
			path.join(ProjectFolder(), "ObjCommon")
		}
	end
end

function ObjCommon:link(links)
	links:add(self:name())
	links:linkto(Utils)
	links:linkto(Common)
	links:linkto(minizip)
end

function ObjCommon:use()
	
end

function ObjCommon:name()
	return "ObjCommon"
end

function ObjCommon:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ObjCommon/**.h"), 
			path.join(folder, "ObjCommon/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjCommon")
			}
		}
		
        self:include(includes)
		Utils:include(includes)
end
