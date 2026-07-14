antlr4 = {}

local ANTLR4_DEFAULT_VERSION = "4.13.2"

function antlr4:include(includes)
    if includes:handle(self:name()) then
        includedirs {
            path.join(ThirdPartyFolder(), "antlr4/runtime/Cpp/runtime/src")
        }
    end
end

function antlr4:link(links)
    links:add(self:name())
end

function antlr4:use()

end

function antlr4:name()
    return "antlr4"
end

function antlr4:project()
    local folder = ThirdPartyFolder()

    project(self:name())
        targetdir(TargetDirectoryLib)
        location "%{wks.location}/thirdparty/%{prj.name}"
        kind "StaticLib"
        language "C++"

        files {
            path.join(folder, "antlr4/runtime/Cpp/runtime/src/**.cpp"),
            path.join(folder, "antlr4/runtime/Cpp/runtime/src/**.h")
        }

        vpaths {
            ["*"] = {
                path.join(folder, "antlr4/runtime/Cpp/runtime/src")
            }
        }

        defines { "ANTLR4CPP_STATIC" }

        self:include(Includes:create())

        warnings "off"
end

function antlr4:jarDir()
    return path.join(BuildFolder(), "thirdparty/antlr4")
end

function antlr4:installJar()
    local jarDir = self:jarDir()
    local versionFile = path.join(jarDir, "antlr4-jar-version.txt")
    if not os.isdir(jarDir) then
        os.mkdir(jarDir)
    end

    local installedVersion = io.readfile(versionFile) or ""
    if installedVersion ~= "" then
        local installedJarPath = path.join(jarDir, "antlr-" .. installedVersion .. "-complete.jar")
        if os.isfile(installedJarPath) then
            return installedJarPath
        end
    end

    local targetVersion = ANTLR4_DEFAULT_VERSION
    local jarName = "antlr-" .. targetVersion .. "-complete.jar"
    local jarPath = path.join(jarDir, jarName)
    if os.isfile(jarPath) then
        io.writefile(versionFile, targetVersion)
        return jarPath
    end

    local downloadUrl = "https://www.antlr.org/download/" .. jarName
    print("Downloading ANTLR4 " .. targetVersion .. " jar...")
    function progress(total, current)
        if total > 0 then
            local ratio = current / total
            ratio = math.min(math.max(ratio, 0), 1)
            local percent = math.floor(ratio * 100)
            io.write("\rDownload progress (" .. percent .. "%/100%)")
        else
            io.write("\rDownloaded " .. current .. " bytes...")
        end
        io.stdout:flush()
    end

    local result_str, response_code = http.download(downloadUrl, jarPath, { progress = progress })
    io.write("\n")

    if result_str ~= "OK" then
        premake.error("Failed to download ANTLR4 jar from " .. downloadUrl .. ": " .. result_str)
    end

    io.writefile(versionFile, targetVersion)
    return jarPath
end

function antlr4:generateGrammar()
    local folder = ProjectFolder()
    local buildFolder = BuildFolder()
    local grammars = os.matchfiles(path.join(folder, "**.g4"))

    if #grammars == 0 then
        return
    end

    local jar = path.getabsolute(self:installJar())

    print("Running antlr4 generation...")

    for _, grammar in ipairs(grammars) do
        local rel = path.getrelative(folder, grammar)
        local outdir = path.getabsolute(path.join(buildFolder, "src", path.getdirectory(rel)))
        local cmd = "java -jar \"" .. jar .. "\" -Dlanguage=Cpp -o \"" .. outdir .. "\" \"" .. path.getabsolute(grammar) .. "\""

        if not os.isdir(outdir) then
            os.mkdir(outdir)
        end
        os.execute(cmd)

        prebuildcommands { cmd }

        files {
            path.join(outdir, "**.h"),
            path.join(outdir, "**.cpp")
        }

        includedirs { outdir }
    end
end
