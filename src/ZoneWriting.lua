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
	links:linkto(Cryptography)
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
			path.join(folder, "ZoneWriting/**.cpp"),
			ZoneCode:allWriteFiles()
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "ZoneWriting"),
				path.join(BuildFolder(), "src/ZoneCode")
			}
		}
		
        self:include(includes)
        Cryptography:include(includes)
        Utils:include(includes)
		zlib:include(includes)
		ZoneCode:include(includes)

		ZoneCode:use()
end
