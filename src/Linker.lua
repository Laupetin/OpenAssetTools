Linker = {}

function Linker:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "Linker")
		}
	end
end

function Linker:link(links)
    
end

function Linker:use()
    dependson(self:name())
end

function Linker:name()
	return "Linker"
end

function Linker:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "Linker/**.h"), 
			path.join(folder, "Linker/**.cpp") 
		}
		
		self:include(includes)
		Utils:include(includes)
        ZoneLoading:include(includes)
        ObjCompiling:include(includes)
        ObjLoading:include(includes)
        ObjWriting:include(includes)
        ZoneWriting:include(includes)

		Raw:use()

		links:linkto(Utils)
		links:linkto(ObjCompiling)
		links:linkto(ZoneLoading)
		links:linkto(ZoneWriting)
		links:linkto(ObjLoading)
		links:linkto(ObjWriting)
		links:linkall()
end
