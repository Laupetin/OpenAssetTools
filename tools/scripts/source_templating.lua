function useSourceTemplating(projectName)
    local projectFolder = path.join(ProjectFolder(), projectName)
    local templateFiles = os.matchfiles(path.join(projectFolder, "**.template"))

    local createdFiles = {}

    for i = 1, #templateFiles do
        local templateFile = templateFiles[i]
        local relativeTemplatePath = path.getrelative(projectFolder, templateFile)
        local relativeResultPath = path.replaceextension(relativeTemplatePath, "")
        local resultExtension = path.getextension(relativeResultPath)

        local data = io.readfile(templateFile)
        local gameOptionsStart, gameOptionsCount = string.find(data, "#options%s+GAME%s*%(")

        if gameOptionsStart == nil then
            error("Source template " .. relativeTemplatePath .. " must define an option called GAME")
        end

        local gameOptionsPos, gameOptionsLenPlusOne = string.find(data, "[%a%d%s,]+%)", gameOptionsStart + gameOptionsCount)

        if gameOptionsPos ~= gameOptionsStart + gameOptionsCount then
            error("Source template " .. relativeTemplatePath .. " must define an option called GAME")
        end

        local gameOptions = string.sub(data, gameOptionsPos, gameOptionsLenPlusOne - 1)
        local games = string.explode(gameOptions, ",%s*")

        files {
            templateFile
        }

        filter("files:" .. templateFile)
            buildmessage("Templating source file " .. relativeTemplatePath)
            buildinputs {
                TargetDirectoryBuildTools .. "/" .. ExecutableByOs('RawTemplater')
            }
            buildcommands {
                '"' .. TargetDirectoryBuildTools .. '/' .. ExecutableByOs('RawTemplater') .. '"' 
                .. ' -o "%{prj.location}/"'
                .. " %{file.relpath}"
            }
            for i = 1, #games do
                local gameName = games[i]
                local outputFileName = path.replaceextension(path.replaceextension(relativeResultPath, "") .. gameName, resultExtension)
                local outputFile = "%{prj.location}/Game/" .. gameName .. "/" .. outputFileName

                table.insert(createdFiles, outputFile)

                buildoutputs {
                    outputFile
                }
            end
        filter {}

		includedirs {
			"%{prj.location}"
		}

        files {
            createdFiles
        }
        
        RawTemplater:use()
    end
end
