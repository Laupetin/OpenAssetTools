ImageConverter = {}

function ImageConverter:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "ImageConverter")
		}
	end
end

function ImageConverter:link(links)
    
end

function ImageConverter:use()
    dependson(self:name())
end

function ImageConverter:name()
	return "ImageConverter"
end

function ImageConverter:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "ImageConverter/**.h"), 
			path.join(folder, "ImageConverter/**.cpp") 
		}
		
		self:include(includes)
		Utils:include(includes)
        ObjLoading:include(includes)
        ObjWriting:include(includes)

		Raw:use()

		links:linkto(Utils)
		links:linkto(ObjLoading)
		links:linkto(ObjWriting)
		links:linkall()
end
