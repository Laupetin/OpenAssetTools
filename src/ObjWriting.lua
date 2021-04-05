ObjWriting = {}

function ObjWriting:include(includes)
	if includes:handle(self:name()) then
		ObjCommon:include(includes)
		ObjLoading:include(includes)
		ZoneCommon:include(includes)
		includedirs {
			path.join(ProjectFolder(), "ObjWriting")
		}
	end
end

function ObjWriting:link(links)
	links:add(self:name())
	links:linkto(Utils)
	links:linkto(ObjCommon)
	links:linkto(ObjLoading)
	links:linkto(ZoneCommon)
	links:linkto(minilzo)
	links:linkto(minizip)
end

function ObjWriting:use()
	
end

function ObjWriting:name()
	return "ObjWriting"
end

function ObjWriting:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ObjWriting/**.h"), 
			path.join(folder, "ObjWriting/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjWriting")
			}
		}
		
        self:include(includes)
		Utils:include(includes)
		minilzo:include(includes)
		minizip:include(includes)
end
