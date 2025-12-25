Catch2Common = {}

function Catch2Common:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "Catch2Common")
		}
	end
end

function Catch2Common:link(links)
	links:add(self:name())
	links:linkto(catch2)
	links:linkto(Utils)
end

function Catch2Common:use()
	
end

function Catch2Common:name()
    return "Catch2Common"
end

function Catch2Common:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "StaticLib"
		language "C++"
		
		files {
			path.join(folder, "Catch2Common/**.h"), 
			path.join(folder, "Catch2Common/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "Catch2Common")
			}
		}
		
		self:include(includes)
		catch2:include(includes)
		Utils:include(includes)

		links:linkto(catch2)
		links:linkto(Utils)
		links:linkall()
end
