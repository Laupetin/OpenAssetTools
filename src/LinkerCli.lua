LinkerCli = {}

function LinkerCli:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "LinkerCli")
		}
	end
end

function LinkerCli:link(links)
    
end

function LinkerCli:use()
    dependson(self:name())
end

function LinkerCli:name()
	return "LinkerCli"
end

function LinkerCli:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryBin)
		targetname "Linker"
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "LinkerCli/**.h"), 
			path.join(folder, "LinkerCli/**.cpp") 
		}
		
		self:include(includes)
		Utils:include(includes)
		Linking:include(includes)

		Raw:use()

		links:linkto(Utils)
		links:linkto(Linking)
		links:linkall()
end
