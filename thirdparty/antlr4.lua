antlr4 = {}

function antlr4:include(includes)
    if includes:handle(self:name()) then
        includedirs {
            path.join(ThirdPartyFolder(), "antlr4/runtime/Cpp/runtime/src"),
            path.join(ThirdPartyFolder(), "antlr4/runtime/Cpp/runtime/src/antlr4-runtime")
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

function antlr4:generateGrammar()
    local folder = ProjectFolder()
    local grammars = os.matchfiles(path.join(folder, "**.g4"))
    local jar = path.getabsolute("thirdparty/antlr4/tool/target/antlr-4.13.2-complete.jar")

    if #grammars == 0 then
        return
    end

	if #grammars > 0 then
		print("Running antlr4 generation...")
	end

    for _, grammar in ipairs(grammars) do
        -- local base = path.getbasename(grammar)
		-- print("Generated " .. base .. "Lexer.cpp/h...")
		-- print("Generated " .. base .. "Parser.cpp/h...")
		-- print("Generated " .. base .. "BaseVisitor.cpp/h...")
		-- print("Generated " .. base .. "Visitor.cpp/h...")

        local rel = path.getrelative(path.join(folder, "ObjLoading"), grammar)
        local outdir = path.getabsolute(path.join(BuildFolder(), "src/ObjLoading", path.getdirectory(rel)))
		local cmd = "java -jar \"" .. path.getabsolute(jar) .. "\" -Dlanguage=Cpp -o \"" .. path.getabsolute(outdir) .. "\" \"" .. path.getabsolute(grammar) .. "\""
		
        prebuildcommands {
            cmd
        }

        files {
            path.join(outdir, "**.h"),
            path.join(outdir, "**.cpp")
        }

        includedirs { outdir }
    end
end
