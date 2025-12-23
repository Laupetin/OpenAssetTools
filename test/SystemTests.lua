SystemTests = {}

function SystemTests:include(includes)
    if includes:handle(self:name()) then
		includedirs {
			path.join(TestFolder(), "SystemTests")
		}
	end
end

function SystemTests:link(links)
	
end

function SystemTests:use()
	
end

function SystemTests:name()
    return "SystemTests"
end

function SystemTests:project()
	local folder = TestFolder()
	local includes = Includes:create()
	local links = Links:create()

	project(self:name())
        targetdir(TargetDirectoryTest)
		location "%{wks.location}/test/%{prj.name}"
		kind "ConsoleApp"
		language "C++"
		
		files {
			path.join(folder, "SystemTests/**.h"), 
			path.join(folder, "SystemTests/**.cpp")
		}
		
		self:include(includes)
		Catch2Common:include(includes)
		Utils:include(includes)
        ZoneLoading:include(includes)
        ZoneWriting:include(includes)
        ObjLoading:include(includes)
        ObjCompiling:include(includes)
        ObjWriting:include(includes)
        Linking:include(includes)
        Unlinking:include(includes)
		catch2:include(includes)

		Raw:use()

		links:linkto(Utils)
		links:linkto(ZoneLoading)
		links:linkto(ZoneWriting)
		links:linkto(ObjLoading)
		links:linkto(ObjCompiling)
		links:linkto(ObjWriting)
		links:linkto(catch2)
		links:linkto(Catch2Common)
		links:linkto(Linking)
		links:linkto(Unlinking)
		links:linkall()
end
