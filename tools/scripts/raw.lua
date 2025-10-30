Raw = {}

function Raw:include(includes)
	if includes:handle(self:name()) then
		Utils:include(includes)
		includedirs {
			RawFolder()
		}
	end
end

function Raw:link(links)
end

function Raw:use()
	dependson(self:name())
end

function Raw:name()
	return "Raw"
end

function Raw:project()
	local folder = RawFolder()

	project(self:name())
        targetdir(TargetDirectoryBin)
		location "%{wks.location}/raw"
		kind "Utility"
		
		files {
			path.join(folder, "*/**"), 
			path.join(folder, "*/**") 
		}
		
        vpaths {
			["*"] = {
				folder
			}
		}

        RawTemplater:use()

		filter "files:not **/*.template"
			buildmessage 'Copying rawfile %{file.relpath}'
			buildcommands {
				-- Relpath contains two .. so build/raw is getting reverted in the target path
				"{MKDIR} \"%{cfg.targetdir}/build/raw/%{file.reldirectory}\"",
				"{COPYFILE} \"%{file.relpath}\" \"%{cfg.targetdir}/build/raw/%{file.relpath}\""
			}
			buildoutputs {
				"%{cfg.targetdir}/build/raw/%{file.relpath}"
			}
		filter {}

		filter "files:**/*.template"
			buildmessage 'Templating %{file.relpath}'
            buildinputs {
                TargetDirectoryBuildTools .. "/" .. ExecutableByOs('RawTemplater')
            }
			buildcommands {
				'"' .. TargetDirectoryBuildTools .. '/' .. ExecutableByOs('RawTemplater') .. '"' 
				.. " --no-color"
				.. " -o %{cfg.targetdir}/build/raw/%{file.reldirectory}"
				.. " --build-log \"%{prj.location}/build/%{file.relpath}.log\""
				.. " %{file.relpath}"
			}
			buildoutputs {
				"%{prj.location}/build/%{file.relpath}.log"
			}
		filter {}
end
