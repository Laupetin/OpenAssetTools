catch2 = {}

function catch2:include(includes)
	if includes:handle(self:name()) then
		includedirs {
			path.join(ThirdPartyFolder(), "catch2", "src"),
			"%{wks.location}/thirdparty/catch2"
		}
	end
end

function catch2:link(links)
	links:add(self:name())
end

function catch2:use()
	
end

function catch2:name()
    return "catch2"
end

function catch2:project()
	local folder = ThirdPartyFolder()
	local includes = Includes:create()

	self:writeUserConfig()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/thirdparty/%{prj.name}"
		kind "StaticLib"
		language "C++"

		files { 
			path.join(folder, "catch2/src/**.cpp"),
			path.join(folder, "catch2/src/**.hpp"),
			"%{wks.location}/thirdparty/%{prj.name}/**.cpp",
			"%{wks.location}/thirdparty/%{prj.name}/**.hpp"
        }
		
        vpaths {
			["*"] = {
				path.join(folder, "catch2/src"),
				"%{wks.location}/thirdparty/%{prj.name}"
			}
		}
        
        defines {
            "DO_NOT_USE_WMAIN",
			"CATCH_AMALGAMATED_CUSTOM_MAIN",
            "_CRT_SECURE_NO_WARNINGS"
        }
		
        self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end

function catch2:writeUserConfig()
	local folder = ThirdPartyFolder()
	local inputFileName = path.join(folder, "catch2/src/catch2/catch_user_config.hpp.in")
	local outputFileName = path.getabsolute("build/thirdparty/catch2/catch2/catch_user_config.hpp")

	local inputInfo = os.stat(inputFileName)
	local outputInfo = os.stat(outputFileName)

	if inputInfo == nil then
		return
	end

	if outputInfo ~= nil and inputInfo.mtime <= outputInfo.mtime then
		return
	end

	local fileData = io.readfile(inputFileName)
	fileData = string.gsub(fileData, '#cmakedefine[^\n]*', '')
	fileData = string.gsub(fileData, '@CATCH_CONFIG_CONSOLE_WIDTH@', '80')
	fileData = string.gsub(fileData, '@CATCH_CONFIG_DEFAULT_REPORTER@', 'console')

	os.mkdir(path.getdirectory(outputFileName))
	os.writefile_ifnotequal(fileData, outputFileName)
end