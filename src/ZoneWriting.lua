ZoneWriting = {}

function ZoneWriting:include()
	if References:include("ZoneWriting") then
		ZoneCommon:include()
		includedirs {
			path.join(ProjectFolder(), "ZoneWriting")
		}
	end
end

function ZoneWriting:link()
	if References:link("ZoneWriting") then
		Crypto:link()
		Utils:link()
		ZoneCommon:link()
		zlib:link()
		links {
			"ZoneWriting"
		}
	end
end

function ZoneWriting:use()
	
end

function ZoneWriting:project()
	References:reset()
	local folder = ProjectFolder();

	project "ZoneWriting"
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
		
        self:include()
        Crypto:include()
        Utils:include()
		zlib:include()

		ZoneCode:use()
end
