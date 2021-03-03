ZoneWriting = {}

function ZoneWriting:include(includes)
	if includes:handle(self:name()) then
		ZoneCommon:include(includes)
		includedirs {
			path.join(ProjectFolder(), "ZoneWriting")
		}
	end
end

function ZoneWriting:link(links)
	links:add(self:name())
	links:linkto(Crypto)
	links:linkto(Utils)
	links:linkto(ZoneCommon)
	links:linkto(zlib)
end

function ZoneWriting:use()
	
end

function ZoneWriting:name()
	return "ZoneWriting"
end

function ZoneWriting:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "ZoneWriting/**.h"), 
			path.join(folder, "ZoneWriting/**.cpp") 
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ZoneWriting")
			}
		}
		
        self:include(includes)
        Crypto:include(includes)
        Utils:include(includes)
		zlib:include(includes)

		ZoneCode:use()
end
