ObjWriting = {}

function ObjWriting:include(includes)
	if includes:handle(self:name()) then
		ObjCommon:include(includes)
		ObjImage:include(includes)
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
	links:linkto(ObjImage)
	links:linkto(ObjLoading)
	links:linkto(ZoneCommon)
	links:linkto(minizip)
	links:linkto(libtomcrypt)
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

		useSourceTemplating("ObjWriting")
		
        self:include(includes)
		Utils:include(includes)
		minizip:include(includes)
		eigen:include(includes)
		json:include(includes)
		libtomcrypt:include(includes)

end
