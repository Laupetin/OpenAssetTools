antlr4 = {}

local ANTLR4_VERSION = "4.13.2"

function antlr4:include(includes)
    if includes:handle(self:name()) then
        includedirs {
            path.join(ThirdPartyFolder(), "antlr4/runtime/Cpp/runtime/src")
        }
        defines { "ANTLR4CPP_STATIC" }
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
	local includes = Includes:create()

    project(self:name())
        targetdir(TargetDirectoryLib)
        location "%{wks.location}/thirdparty/%{prj.name}"
        kind "StaticLib"
        language "C++"

        files {
            path.join(folder, "antlr4/runtime/Cpp/runtime/src/**.cpp"),
            path.join(folder, "antlr4/runtime/Cpp/runtime/src/**.h")
        }

		if os.host() == "windows" then
			self:installJar()
		end

        defines { "ANTLR4CPP_STATIC" }

		self:include(includes)

		-- Disable warnings. They do not have any value to us since it is not our code.
		warnings "off"
end

function antlr4:jarPath()
    return path.join(BuildFolder(), "thirdparty/antlr4", "antlr-" .. ANTLR4_VERSION .. "-complete.jar")
end

-- Downloads and installs latest ANTLR4 jar
function antlr4:installJar()
	local truth_hash = "7df86c341abb175a0f4b76a7845074cc45f82ff8"
    local jarDir = path.join(BuildFolder(), "thirdparty/antlr4")
    local versionFile = path.join(jarDir, "antlr4-jar-version.txt")
    local jarPath = self:jarPath()
    local jarName = "antlr-" .. ANTLR4_VERSION .. "-complete.jar"
    local url = "https://www.antlr.org/download/" .. jarName

    if not os.isdir(jarDir) then
        os.mkdir(jarDir)
    end

    local installedVersion = io.readfile(versionFile)
    if installedVersion == ANTLR4_VERSION and os.isfile(jarPath) then
        return
    end

    local function progress(total, current)
		local ratio = current / total;
		ratio = math.min(math.max(ratio, 0), 1);
		local percent = math.floor(ratio * 100);
		io.write("\rDownload progress (" .. percent .. "%/100%)")
    end

    print("Downloading ANTLR4 " .. ANTLR4_VERSION .. " jar...")
    local result_str, response_code = http.download(url, jarPath, { 
		progress = progress 
	})

    io.write("\n")

    if result_str ~= "OK" then
        premake.error("Failed to download ANTLR4 jar from " .. url .. ": " .. result_str)
    end

    local hash = string.lower(string.sha1(io.readfile(jarPath)))
    if hash ~= truth_hash then
        os.remove(jarPath)
        premake.error("ANTLR4 jar SHA1 mismatch (expected " .. truth_hash .. ", got " .. hash .. ")")
    end

    io.writefile(versionFile, ANTLR4_VERSION)
end

-- Adds all .g4 grammar files found recursively under dir to the current project
function antlr4:addGrammarsInDir(dir)
    for _, grammarFile in ipairs(os.matchfiles(path.join(dir, "**.g4"))) do
        self:addGrammar(grammarFile)
    end
end

-- Adds a .g4 grammar file to the current project
function antlr4:addGrammar(grammarFile)
    local absGrammar = path.getabsolute(grammarFile)
    local grammarName = path.getbasename(absGrammar)
    local absOutputDir = path.getdirectory(absGrammar)

    files { grammarFile }

    if _OPTIONS["antlr4"] then
        local jar = path.getabsolute(self:jarPath())
        local cmd = "java -jar \"" .. jar .. "\" -Dlanguage=Cpp -o \"" .. absOutputDir .. "\" \"" .. absGrammar .. "\""

        filter("files:" .. grammarFile)
            buildmessage("Generating ANTLR4 parser for " .. grammarName .. ".g4")
            buildinputs  { absGrammar }
            buildcommands { cmd }
            buildoutputs {
                path.join(absOutputDir, grammarName .. "Lexer.h"),
                path.join(absOutputDir, grammarName .. "Lexer.cpp"),
                path.join(absOutputDir, grammarName .. "Parser.h"),
                path.join(absOutputDir, grammarName .. "Parser.cpp"),
                path.join(absOutputDir, grammarName .. "Listener.h"),
                path.join(absOutputDir, grammarName .. "Listener.cpp"),
                path.join(absOutputDir, grammarName .. "BaseListener.h"),
                path.join(absOutputDir, grammarName .. "BaseListener.cpp"),
            }
        filter {}
    end
end
