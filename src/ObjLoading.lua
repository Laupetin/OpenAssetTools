ObjLoading = {}

function ObjLoading:include(includes)
	if includes:handle(self:name()) then
		ObjCommon:include(includes)
		ObjImage:include(includes)
		ZoneCommon:include(includes)
		includedirs {
			path.join(ProjectFolder(), "ObjLoading")
		}
	end
end

function ObjLoading:link(links)
	links:add(self:name())
	links:linkto(Utils)
	links:linkto(ObjCommon)
	links:linkto(ObjImage)
	links:linkto(ZoneCommon)
	links:linkto(minilzo)
	links:linkto(minizip)
	links:linkto(zlib)
	links:linkto(libtomcrypt)
	links:linkto(ufbx)
end

function ObjLoading:use()
	
end

function ObjLoading:name()
	return "ObjLoading"
end

function ObjLoading:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ObjLoading/**.h"), 
			path.join(folder, "ObjLoading/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ObjLoading")
			}
		}

		ObjCommon:use()
		useSourceTemplating("ObjLoading")
		
		self:include(includes)
		Cryptography:include(includes)
		Utils:include(includes)
		minilzo:include(includes)
		minizip:include(includes)
		zlib:include(includes)
		eigen:include(includes)
		json:include(includes)
		libtomcrypt:include(includes)
		ufbx:include(includes)
end
