UnlinkerCli = {}

function UnlinkerCli:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "UnlinkerCli")
		}
	end
end

function UnlinkerCli:link(links)
    
end

function UnlinkerCli:use()
    dependson(self:name())
end

function UnlinkerCli:name()
	return "UnlinkerCli"
end

function UnlinkerCli:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryBin)
		targetname "Unlinker"
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "UnlinkerCli/**.h"), 
			path.join(folder, "UnlinkerCli/**.cpp") 
		}
		
		self:include(includes)
		Utils:include(includes)
		Unlinking:include(includes)

		Raw:use()

		links:linkto(Utils)
		links:linkto(Unlinking)
		links:linkall()
end
