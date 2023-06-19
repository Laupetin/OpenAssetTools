Decompiler = {}

function Decompiler:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "Decompiler")
		}
	end
end

function Decompiler:link(links)
    
end

function Decompiler:use()
    dependson(self:name())
end

function Decompiler:name()
	return "Decompiler"
end

function Decompiler:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "Decompiler/**.h"), 
			path.join(folder, "Decompiler/**.cpp") 
		}
		
		self:include(includes)
		Utils:include(includes)

		Raw:use()

		links:linkto(Utils)
		links:linkall()
end
