XMemCompress = {}

function XMemCompress:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "XMemCompress")
		}
	end
end

function XMemCompress:link(links)
	links:add(self:name())
	links:linkto(Utils)
	links:linkto(lzx)
end

function XMemCompress:use()
	
end

function XMemCompress:name()
	return "XMemCompress"
end

function XMemCompress:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "XMemCompress/**.h"), 
			path.join(folder, "XMemCompress/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "XMemCompress")
			}
		}
		
		self:include(includes)
		Utils:include(includes)
		lzx:include(includes)
end
