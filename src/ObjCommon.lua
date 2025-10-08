ObjCommon = {}

function ObjCommon:include(includes)
	if includes:handle(self:name()) then
		Common:include(includes)
		json:include(includes)
		minizip:include(includes)
		Parser:include(includes)
		includedirs {
			path.join(ProjectFolder(), "ObjCommon"),
            "%{wks.location}/src/ObjCommon"
		}
	end
end

function ObjCommon:link(links)
	links:add(self:name())
	links:linkto(Utils)
	links:linkto(Common)
	links:linkto(minizip)
	links:linkto(Parser)
end

function ObjCommon:use()
	dependson(self:name())
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

		useSourceTemplating("ObjCommon")
		
        self:include(includes)
		Utils:include(includes)
		eigen:include(includes)
end
