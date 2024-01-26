local BuildSubFolderFolder = "premake"
local HeaderFileName = "GitVersion.h"

function GetGitVersion()
    result, errorCode = os.outputof("git describe --tags")

    if errorCode == 0 then
        return result
    end

    return "Unknown"
end

function GetVersionHeaderFolder()
    return path.join(BuildFolder(), BuildSubFolderFolder)
end

function WriteVersionHeader()
    local folder = GetVersionHeaderFolder()
    local file = path.join(folder, HeaderFileName)
    local content = string.format([[
#pragma once

#define GIT_VERSION "%s"
    ]], GetGitVersion())

    if os.isdir(folder) ~= True then
        os.mkdir(folder)
    end

    local ok, err = os.writefile_ifnotequal(content, file)

    if ok == -1 then
        error("Could not create version file: " .. err)
    end
end
