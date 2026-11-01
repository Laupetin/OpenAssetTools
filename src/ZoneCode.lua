ZoneCode = {}

ZoneCode.Games = {
    IW3 = {
        Variants = {
            "pc"
        },
        Assets = {
            "PhysPreset",
            "XAnim",
            "XModel",
            "Material",
            "TechniqueSet",
            "Image",
            "Sound",
            "SoundCurve",
            "LoadedSound",
            "ClipMapPvs",
            "ComWorld",
            "GameWorldSp",
            "GameWorldMp",
            "MapEnts",
            "GfxWorld",
            "LightDef",
            "Font",
            "MenuList",
            "Menu",
            "Localize",
            "Weapon",
            "Fx",
            "ImpactFx",
            "RawFile",
            "StringTable"
        }
    },

    IW4 = {
        Variants = {
            "pc32",
            "pc64"
        },
        Assets = {
            "PhysPreset",
            "PhysCollMap",
            "XAnim",
            "XModel",
            "Material",
            "PixelShader",
            "VertexShader",
            "VertexDecl",
            "TechniqueSet",
            "Image",
            "Sound",
            "SoundCurve",
            "LoadedSound",
            "ClipMapMp",
            "ComWorld",
            "GameWorldSp",
            "GameWorldMp",
            "MapEnts",
            "FxWorld",
            "GfxWorld",
            "LightDef",
            "Font",
            "MenuList",
            "Menu",
            "Localize",
            "Weapon",
            "Fx",
            "ImpactFx",
            "RawFile",
            "StringTable",
            "Leaderboard",
            "StructuredDataDef",
            "Tracer",
            "Vehicle",
            "AddonMapEnts"
        }
    },

    IW5 = {
        Variants = {
            "pc32",
            "pc64"
        },
        Assets = {
            "PhysPreset",
            "PhysCollMap",
            "XAnim",
            "XModelSurfs",
            "XModel",
            "Material",
            "PixelShader",
            "VertexShader",
            "VertexDecl",
            "TechniqueSet",
            "Image",
            "Sound",
            "SoundCurve",
            "LoadedSound",
            "ClipMap",
            "ComWorld",
            "GlassWorld",
            "PathData",
            "VehicleTrack",
            "MapEnts",
            "FxWorld",
            "GfxWorld",
            "LightDef",
            "Font",
            "MenuList",
            "Menu",
            "Localize",
            "Attachment",
            "Weapon",
            "Fx",
            "ImpactFx",
            "SurfaceFx",
            "RawFile",
            "Script",
            "StringTable",
            "Leaderboard",
            "StructuredDataDef",
            "Tracer",
            "Vehicle",
            "AddonMapEnts",
        }
    },

    QOS = {
        Variants = {
            "pc"
        },
        Assets = {
            "PhysPreset",
            "PhysConstraints",
            "DestructibleDef",
            "XAnim",
            "XModel",
            "Material",
            "TechniqueSet",
            "Image",
            "Sound",
            "SoundCurve",
            "ClipMapMp",
            "ComWorld",
            "GameWorldSp",
            "GameWorldMp",
            "MapEnts",
            "GfxWorld",
            "LightDef",
            "Font",
            "MenuList",
            "Menu",
            "Localize",
            "Weapon",
            "Fx",
            "ImpactFx",
            "RawFile",
            "StringTable",
            "XmlTree",
            "SceneAnimation",
            "Cutscene",
            "CustomCamera",
        }
    },

    T4 = {
        Variants = {
            "pc"
        },
        Assets = {
            "PhysPreset",
            "PhysConstraints",
            "DestructibleDef",
            "XAnim",
            "XModel",
            "Material",
            "TechniqueSet",
            "Image",
            "Sound",
            "SoundDriverGlobals",
            "LoadedSound",
            "ClipMapPvs",
            "ComWorld",
            "GameWorldSp",
            "GameWorldMp",
            "MapEnts",
            "GfxWorld",
            "LightDef",
            "Font",
            "MenuList",
            "Menu",
            "Localize",
            "Weapon",
            "Fx",
            "ImpactFx",
            "RawFile",
            "StringTable",
            "PackIndex",
        }
    },

    T5 = {
        Variants = {
            "pc"
        },
        Assets = {
            "PhysPreset",
            "PhysConstraints",
            "DestructibleDef",
            "XAnim",
            "XModel",
            "Material",
            "TechniqueSet",
            "Image",
            "SoundBank",
            "SoundPatch",
            "ClipMapPvs",
            "ComWorld",
            "GameWorldSp",
            "GameWorldMp",
            "MapEnts",
            "GfxWorld",
            "LightDef",
            "Font",
            "MenuList",
            "Menu",
            "Localize",
            "Weapon",
            "SoundDriverGlobals",
            "Fx",
            "ImpactFx",
            "RawFile",
            "StringTable",
            "PackIndex",
            "XGlobals",
            "DDL",
            "Glasses",
            "EmblemSet"
        }
    },

    T6 = {
        Variants = {
            "pc"
        },
        Assets = {
            "PhysPreset",
            "PhysConstraints",
            "DestructibleDef",
            "XAnim",
            "XModel",
            "Material",
            "TechniqueSet",
            "Image",
            "SoundBank",
            "SoundPatch",
            "ClipMapPvs",
            "ComWorld",
            "GameWorldSp",
            "GameWorldMp",
            "MapEnts",
            "GfxWorld",
            "LightDef",
            "Font",
            "FontIcon",
            "MenuList",
            "Menu",
            "Localize",
            "Weapon",
            "Attachment",
            "AttachmentUnique",
            "WeaponCamo",
            "SoundDriverGlobals",
            "Fx",
            "ImpactFx",
            "RawFile",
            "StringTable",
            "Leaderboard",
            "XGlobals",
            "DDL",
            "Glasses",
            "EmblemSet",
            "Script",
            "KeyValuePairs",
            "Vehicle",
            "MemoryBlock",
            "AddonMapEnts",
            "Tracer",
            "SkinnedVerts",
            "Qdb",
            "Slug",
            "FootstepTable",
            "FootstepFxTable",
            "ZBarrier"
        }
    }
}

function ZoneCode:allTestFiles()
    result = {}

    for game, gameData in pairs(self.Games) do
        local gameLower = string.lower(game)

        for i, variantName in ipairs(gameData.Variants) do
            local variantLower = string.lower(variantName)

            -- Per asset
            for j, assetName in ipairs(gameData.Assets) do
                local assetNameLower = string.lower(assetName)
                table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_" .. gameLower .. "_" .. variantName .. "_struct_test.cpp")
            end
        end
    end
    
    return result
end

function ZoneCode:allMarkFiles()
    result = {}

    for game, gameData in pairs(self.Games) do
        local gameLower = string.lower(game)

        for i, variantName in ipairs(gameData.Variants) do
            local variantLower = string.lower(variantName)

            -- Per asset
            for j, assetName in ipairs(gameData.Assets) do
                local assetNameLower = string.lower(assetName)
                table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_" .. gameLower .. "_" .. variantName .. "_mark_db.cpp")
                table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_" .. gameLower .. "_" .. variantName .. "_mark_db.h")
            end
            
            -- Per variant
            table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/AssetMarker_" .. gameLower .. "_" .. variantLower .. ".h")
        end
    end
    
    return result
end

function ZoneCode:allLoadFiles()
    result = {}

    for game, gameData in pairs(self.Games) do
        local gameLower = string.lower(game)

        for i, variantName in ipairs(gameData.Variants) do
            local variantLower = string.lower(variantName)

            -- Per asset
            for j, assetName in ipairs(gameData.Assets) do
                local assetNameLower = string.lower(assetName)
                table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_" .. gameLower .. "_" .. variantName .. "_load_db.cpp")
                table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_" .. gameLower .. "_" .. variantName .. "_load_db.h")
            end
            
            -- Per variant
            table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/AssetLoader_" .. gameLower .. "_" .. variantLower .. ".h")
        end
    end
    
    return result
end

function ZoneCode:allWriteFiles()
    result = {}

    for game, gameData in pairs(self.Games) do
        local gameLower = string.lower(game)

        for i, variantName in ipairs(gameData.Variants) do
            local variantLower = string.lower(variantName)

            -- Per asset
            for j, assetName in ipairs(gameData.Assets) do
                local assetNameLower = string.lower(assetName)
                table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_" .. gameLower .. "_" .. variantName .. "_write_db.cpp")
                table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_" .. gameLower .. "_" .. variantName .. "_write_db.h")
            end
            
            -- Per variant
            table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/AssetWriter_" .. gameLower .. "_" .. variantLower .. ".h")
        end
    end
    
    return result
end

function ZoneCode:include(includes)
	if includes:handle(self:name()) then
		Common:include(includes)
        includedirs {
            path.join(ProjectFolder(), "ZoneCode"),
            "%{wks.location}/src/ZoneCode"
        }
    end
end

function ZoneCode:link(links)
    
end

function ZoneCode:use()
	dependson(self:name())
end

function ZoneCode:name()
    return "ZoneCode"
end

function ZoneCode:project()
	local folder = ProjectFolder()
	local includes = Includes:create()

	project(self:name())
        targetdir(TargetDirectoryLib)
		location "%{wks.location}/src/%{prj.name}"
		kind "Utility"
		
		files {
			path.join(folder, "ZoneCode/**.gen"),
			path.join(folder, "ZoneCode/**.h"),
			path.join(folder, "ZoneCode/**.txt")
        }
		
        vpaths {
			["*"] = {
				path.join(folder, "ZoneCode")
			}
		}

        self:include(includes)
        ZoneCodeGenerator:use()

        filter "files:**.gen"
            buildmessage "Generating ZoneCode for game %{file.basename}"
            buildcommands {
                '"' .. TargetDirectoryBuildTools .. '/' .. ExecutableByOs('ZoneCodeGenerator') .. '"' 
                    .. ' --no-color'
                    .. ' -h "' .. path.join(path.getabsolute(ProjectFolder()), 'ZoneCode/Game/%{file.basename}/%{file.basename}_ZoneCode.h') .. '"'
                    .. ' -c "' .. path.join(path.getabsolute(ProjectFolder()), 'ZoneCode/Game/%{file.basename}/%{file.basename}_Commands.txt') .. '"'
                    .. ' -o "%{wks.location}/src/ZoneCode/Game/%{file.basename}"'
                    .. ' --build-log "%{wks.location}/src/ZoneCode/Game/%{file.basename}.log"'
                    .. ' -g ZoneLoad'
                    .. ' -g ZoneMark'
                    .. ' -g ZoneWrite'
                    .. ' -g AssetStructTests'
            }
            buildinputs {
                path.join(ProjectFolder(), "ZoneCode/Game/%{file.basename}/%{file.basename}_ZoneCode.h"),
                path.join(ProjectFolder(), "ZoneCode/Game/%{file.basename}/%{file.basename}_Commands.txt"),
                path.join(ProjectFolder(), "Common/Game/%{file.basename}/%{file.basename}_Assets.h"),
                TargetDirectoryBuildTools .. "/" .. ExecutableByOs('ZoneCodeGenerator')
            }
            buildoutputs {
                "%{wks.location}/src/ZoneCode/Game/%{file.basename}.log"
            }
        filter {}
end
