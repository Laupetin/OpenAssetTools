ModMan = {}

function ModMan:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "ModMan")
		}
	end
end

function ModMan:link(links)
    
end

function ModMan:use()
    dependson(self:name())
end

function ModMan:name()
	return "ModMan"
end

function ModMan:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "WindowedApp"
		language "C++"
		
		files {
			path.join(folder, "ModMan/**.h"), 
			path.join(folder, "ModMan/**.cpp") 
		}

		includedirs {
			"%{prj.location}"
		}
		
		self:include(includes)
		webview:include(includes)

		links:linkto(webview)
		links:linkall()
end
