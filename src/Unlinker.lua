Unlinker = {}

function Unlinker:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "Unlinker")
		}
	end
end

function Unlinker:link(links)
    
end

function Unlinker:use()
    dependson(self:name())
end

function Unlinker:name()
	return "Unlinker"
end

function Unlinker:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "Unlinker/**.h"), 
			path.join(folder, "Unlinker/**.cpp") 
		}
		
		self:include(includes)
		Utils:include(includes)
        ZoneLoading:include(includes)
        ObjLoading:include(includes)
        ObjWriting:include(includes)

		Raw:use()

		links:linkto(Utils)
		links:linkto(ZoneLoading)
		links:linkto(ObjLoading)
		links:linkto(ObjWriting)
		links:linkall()
end
