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

		filter "files:**"
			buildmessage 'Copying rawfile %{file.relpath}'
			buildcommands {
				-- Relpath contains two .. so build/raw is getting reverted in the target path
				"cp \"%{file.relpath}\" \"%{cfg.targetdir}/build/raw/%{file.relpath}\""
			}
			buildoutputs {
				"%{cfg.targetdir}/build/raw/%{file.relpath}"
			}
		filter {}
end
