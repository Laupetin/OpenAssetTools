Compiler = {}

function Compiler:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "Compiler")
		}
	end
end

function Compiler:link(links)
    
end

function Compiler:use()
    dependson(self:name())
end

function Compiler:name()
	return "Compiler"
end

function Compiler:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "Compiler/**.h"), 
			path.join(folder, "Compiler/**.cpp") 
		}
		
		self:include(includes)
		Utils:include(includes)

		Raw:use()

		links:linkto(Utils)
		links:linkall()
end
