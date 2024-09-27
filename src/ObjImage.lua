ObjImage = {}

function ObjImage:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "ObjImage")
		}
	end
end

function ObjImage:link(links)
	links:add(self:name())
	links:linkto(Utils)
end

function ObjImage:use()
	
end

function ObjImage:name()
	return "ObjImage"
end

function ObjImage:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ObjImage/**.h"), 
			path.join(folder, "ObjImage/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjImage")
			}
		}
		
		self:include(includes)
		Utils:include(includes)
end
