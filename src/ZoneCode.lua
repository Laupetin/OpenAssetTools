ZoneCode = {}

ZoneCode.Assets = {
    IW3 = {
        "PhysPreset",
        "XAnimParts",
        "XModel",
        "Material",
        "MaterialTechniqueSet",
        "GfxImage",
        "snd_alias_list_t",
        "SndCurve",
        "LoadedSound",
        "clipMap_t",
        "ComWorld",
        "GameWorldSp",
        "GameWorldMp",
        "MapEnts",
        "GfxWorld",
        "GfxLightDef",
        "Font_s",
        "MenuList",
        "menuDef_t",
        "LocalizeEntry",
        "WeaponDef",
        "FxEffectDef",
        "FxImpactTable",
        "RawFile",
        "StringTable"
    },

    IW4 = {
        "PhysPreset",
        "PhysCollmap",
        "XAnimParts",
        "XModel",
        "Material",
        "MaterialPixelShader",
        "MaterialVertexShader",
        "MaterialVertexDeclaration",
        "MaterialTechniqueSet",
        "GfxImage",
        "snd_alias_list_t",
        "SndCurve",
        "LoadedSound",
        "clipMap_t",
        "ComWorld",
        "GameWorldSp",
        "GameWorldMp",
        "MapEnts",
        "FxWorld",
        "GfxWorld",
        "GfxLightDef",
        "Font_s",
        "MenuList",
        "menuDef_t",
        "LocalizeEntry",
        "WeaponCompleteDef",
        "FxEffectDef",
        "FxImpactTable",
        "RawFile",
        "StringTable",
        "LeaderboardDef",
        "StructuredDataDefSet",
        "TracerDef",
        "VehicleDef",
        "AddonMapEnts"
    },

    T5 = {
        "PhysPreset",
        "PhysConstraints",
        "DestructibleDef",
        "XAnimParts",
        "XModel",
        "Material",
        "MaterialTechniqueSet",
        "GfxImage",
        "SndBank",
        "SndPatch",
        "clipMap_t",
        "ComWorld",
        "GameWorldSp",
        "GameWorldMp",
        "MapEnts",
        "GfxWorld",
        "GfxLightDef",
        "Font_s",
        "MenuList",
        "menuDef_t",
        "LocalizeEntry",
        "WeaponVariantDef",
        "SndDriverGlobals",
        "FxEffectDef",
        "FxImpactTable",
        "RawFile",
        "StringTable",
        "PackIndex",
        "XGlobals",
        "ddlRoot_t",
        "Glasses",
        "EmblemSet"
    },

    T6 = {
        "PhysPreset",
        "PhysConstraints",
        "DestructibleDef",
        "XAnimParts",
        "XModel",
        "Material",
        "MaterialTechniqueSet",
        "GfxImage",
        "SndBank",
        "SndPatch",
        "clipMap_t",
        "ComWorld",
        "GameWorldSp",
        "GameWorldMp",
        "MapEnts",
        "GfxWorld",
        "GfxLightDef",
        "Font_s",
        "FontIcon",
        "MenuList",
        "menuDef_t",
        "LocalizeEntry",
        "WeaponVariantDef",
        "WeaponAttachment",
        "WeaponAttachmentUnique",
        "WeaponCamo",
        "SndDriverGlobals",
        "FxEffectDef",
        "FxImpactTable",
        "RawFile",
        "StringTable",
        "LeaderboardDef",
        "XGlobals",
        "ddlRoot_t",
        "Glasses",
        "EmblemSet",
        "ScriptParseTree",
        "KeyValuePairs",
        "VehicleDef",
        "MemoryBlock",
        "AddonMapEnts",
        "TracerDef",
        "SkinnedVertsDef",
        "Qdb",
        "Slug",
        "FootstepTableDef",
        "FootstepFXTableDef",
        "ZBarrierDef"
    }
}

function ZoneCode:outputForAssets(assetList)
    for i = 1, #assetList do
        local assetNameLower = string.lower(assetList[i])
        buildoutputs {
            "%{wks.location}/src/ZoneCode/Game/%{file.basename}/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_load_db.cpp",
            "%{wks.location}/src/ZoneCode/Game/%{file.basename}/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_load_db.h",
            "%{wks.location}/src/ZoneCode/Game/%{file.basename}/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_write_db.cpp",
            "%{wks.location}/src/ZoneCode/Game/%{file.basename}/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_write_db.h",
            "%{wks.location}/src/ZoneCode/Game/%{file.basename}/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_struct_test.cpp",
        }
    end
end

function ZoneCode:allTestFiles()
    result = {}

    for game, assets in pairs(self.Assets) do
        for i, assetName in ipairs(assets) do
            local assetNameLower = string.lower(assetName)
            table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_struct_test.cpp")
        end
    end
    
    return result
end

function ZoneCode:allLoadFiles()
    result = {}

    for game, assets in pairs(self.Assets) do
        for i, assetName in ipairs(assets) do
            local assetNameLower = string.lower(assetName)
            table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_load_db.cpp")
            table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_load_db.h")
        end
    end
    
    return result
end

function ZoneCode:allWriteFiles()
    result = {}

    for game, assets in pairs(self.Assets) do
        for i, assetName in ipairs(assets) do
            local assetNameLower = string.lower(assetName)
            table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_write_db.cpp")
            table.insert(result, "%{wks.location}/src/ZoneCode/Game/" .. game .. "/XAssets/" .. assetNameLower .. "/" .. assetNameLower .. "_write_db.h")
        end
    end
    
    return result
end

function ZoneCode:include(includes)
	if includes:handle(self:name()) then
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

        ZoneCodeGenerator:use()

        filter "files:**.gen"
            buildmessage "Generating ZoneCode for game %{file.basename}"
            buildcommands {
                '"' .. TargetDirectoryBin .. '/' .. ExecutableByOs('ZoneCodeGenerator') .. '"' 
                    .. ' -h "' .. path.join(path.getabsolute(ProjectFolder()), 'ZoneCode/Game/%{file.basename}/%{file.basename}.h') .. '"'
                    .. ' -c "' .. path.join(path.getabsolute(ProjectFolder()), 'ZoneCode/Game/%{file.basename}/%{file.basename}_Commands.txt') .. '"'
                    .. ' -o "%{wks.location}/src/ZoneCode/Game/%{file.basename}/XAssets"'
                    .. ' -g "*" ZoneLoad'
                    .. ' -g "*" ZoneWrite'
                    .. ' -g "*" AssetStructTests'
            }
            buildinputs {
                path.join(ProjectFolder(), "ZoneCode/Game/%{file.basename}/%{file.basename}.h"),
                path.join(ProjectFolder(), "ZoneCode/Game/%{file.basename}/%{file.basename}_Commands.txt"),
                path.join(ProjectFolder(), "Common/Game/%{file.basename}/%{file.basename}_Assets.h"),
                TargetDirectoryBin .. "/" .. ExecutableByOs('ZoneCodeGenerator')
            }
        filter {}
        
        filter "files:**/IW3.gen"
            self:outputForAssets(self.Assets.IW3)
        filter {}
        
        filter "files:**/IW4.gen"
            self:outputForAssets(self.Assets.IW4)
        filter {}
        
        filter "files:**/T5.gen"
            self:outputForAssets(self.Assets.T5)
        filter {}
        
        filter "files:**/T6.gen"
            self:outputForAssets(self.Assets.T6)
        filter {}
end
