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
			"%{wks.location}/src/ModMan"
		}

		filter { "system:linux", "action:gmake" }
  			buildoptions { "`pkg-config --cflags gtk4 webkitgtk-6.0`" }
  			linkoptions { "`pkg-config --libs gtk4 webkitgtk-6.0`" }
		filter {}
		
		self:include(includes)
		Utils:include(includes)
        ZoneLoading:include(includes)
        ObjLoading:include(includes)
        ObjWriting:include(includes)
		json:include(includes)
		webview:include(includes)

		links:linkto(Utils)
		links:linkto(ZoneLoading)
		links:linkto(ObjLoading)
		links:linkto(ObjWriting)
		links:linkto(webview)
		links:linkall()
end
