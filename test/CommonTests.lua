CommonTests = {}

function CommonTests:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "CommonTests")
		}
	end
end

function CommonTests:link(links)
	
end

function CommonTests:use()
	
end

function CommonTests:name()
    return "CommonTests"
end

function CommonTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "CommonTests/**.h"), 
			path.join(folder, "CommonTests/**.cpp")
		}
		
        vpaths {
			["*"] = {
				path.join(folder, "CommonTests")
			}
		}
		
		self:include(includes)
		Catch2Common:include(includes)
		Common:include(includes)
		catch2:include(includes)

		links:linkto(Common)
		links:linkto(catch2)
		links:linkto(Catch2Common)
		links:linkall()
end
