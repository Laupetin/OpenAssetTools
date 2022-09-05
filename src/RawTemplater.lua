RawTemplater = {}

function RawTemplater:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ProjectFolder(), "RawTemplater")
		}
		Utils:include(includes)
	end
end

function RawTemplater:link(links)

end

function RawTemplater:use()
    dependson(self:name())
end

function RawTemplater:name()
    return "RawTemplater"
end

function RawTemplater:project()
	local folder = ProjectFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
		targetdir(TargetDirectoryBin)
		location "%{wks.location}/src/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "RawTemplater/**.h"), 
			path.join(folder, "RawTemplater/**.cpp") 
		}
        vpaths {
			["*"] = {
				path.join(folder, "RawTemplater"),
			}
		}
		
		self:include(includes)
		Parser:include(includes)

		links:linkto(Parser)
		links:linkall()
end
