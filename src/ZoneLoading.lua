ZoneLoading = {}

function ZoneLoading:include(includes)
	if includes:handle(self:name()) then
		ZoneCommon:include(includes)
		includedirs {
			path.join(ProjectFolder(), "ZoneLoading")
		}
	end
end

function ZoneLoading:link(links)
	links:add(self:name())
	links:linkto(Crypto)
	links:linkto(Utils)
	links:linkto(ZoneCommon)
	links:linkto(zlib)
end

function ZoneLoading:use()
	
end

function ZoneLoading:name()
	return "ZoneLoading"
end

function ZoneLoading:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneLoading/**.h"), 
			path.join(folder, "ZoneLoading/**.cpp"),
			ZoneCode:allLoadFiles()
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ZoneLoading"),
				path.join(BuildFolder(), "src/ZoneCode")
			}
		}
		
        self:include(includes)
        Crypto:include(includes)
		Utils:include(includes)
		zlib:include(includes)
		ZoneCode:include(includes)

		ZoneCode:use()
end
