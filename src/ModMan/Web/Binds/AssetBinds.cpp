#include "AssetBinds.h"

#include "Context/ModManContext.h"
#include "Game/CommonAsset.h"
#include "Web/UiCommunication.h"

#include "Json/JsonExtension.h"
#include <cstdint>

NLOHMANN_JSON_SERIALIZE_ENUM(CommonAssetType,
                             {
                                 {CommonAssetType::PHYS_PRESET,          "phys_preset"         },
                                 {CommonAssetType::XANIM,                "xanim"               },
                                 {CommonAssetType::XMODEL,               "xmodel"              },
                                 {CommonAssetType::MATERIAL,             "material"            },
                                 {CommonAssetType::TECHNIQUE_SET,        "technique_set"       },
                                 {CommonAssetType::IMAGE,                "image"               },
                                 {CommonAssetType::SOUND,                "sound"               },
                                 {CommonAssetType::SOUND_CURVE,          "sound_curve"         },
                                 {CommonAssetType::LOADED_SOUND,         "loaded_sound"        },
                                 {CommonAssetType::CLIP_MAP,             "clip_map"            },
                                 {CommonAssetType::COM_WORLD,            "com_world"           },
                                 {CommonAssetType::GAME_WORLD_SP,        "game_world_sp"       },
                                 {CommonAssetType::GAME_WORLD_MP,        "game_world_mp"       },
                                 {CommonAssetType::MAP_ENTS,             "map_ents"            },
                                 {CommonAssetType::GFX_WORLD,            "gfx_world"           },
                                 {CommonAssetType::LIGHT_DEF,            "light_def"           },
                                 {CommonAssetType::UI_MAP,               "ui_map"              },
                                 {CommonAssetType::FONT,                 "font"                },
                                 {CommonAssetType::MENU_LIST,            "menu_list"           },
                                 {CommonAssetType::MENU,                 "menu"                },
                                 {CommonAssetType::LOCALIZE_ENTRY,       "localize_entry"      },
                                 {CommonAssetType::WEAPON,               "weapon"              },
                                 {CommonAssetType::SOUND_DRIVER_GLOBALS, "sound_driver_globals"},
                                 {CommonAssetType::FX,                   "fx"                  },
                                 {CommonAssetType::IMPACT_FX,            "impact_fx"           },
                                 {CommonAssetType::AI_TYPE,              "ai_type"             },
                                 {CommonAssetType::MP_TYPE,              "mp_type"             },
                                 {CommonAssetType::CHARACTER,            "character"           },
                                 {CommonAssetType::XMODEL_ALIAS,         "xmodel_alias"        },
                                 {CommonAssetType::RAW_FILE,             "raw_file"            },
                                 {CommonAssetType::STRING_TABLE,         "string_table"        },
                                 {CommonAssetType::XMODEL_PIECES,        "xmodel_pieces"       },
                                 {CommonAssetType::PHYS_COLL_MAP,        "phys_coll_map"       },
                                 {CommonAssetType::XMODEL_SURFS,         "xmodel_surfs"        },
                                 {CommonAssetType::PIXEL_SHADER,         "pixel_shader"        },
                                 {CommonAssetType::VERTEX_SHADER,        "vertex_shader"       },
                                 {CommonAssetType::VERTEX_DECL,          "vertex_decl"         },
                                 {CommonAssetType::FX_WORLD,             "fx_world"            },
                                 {CommonAssetType::LEADERBOARD,          "leaderboard"         },
                                 {CommonAssetType::STRUCTURED_DATA_DEF,  "structured_data_def" },
                                 {CommonAssetType::TRACER,               "tracer"              },
                                 {CommonAssetType::VEHICLE,              "vehicle"             },
                                 {CommonAssetType::ADDON_MAP_ENTS,       "addon_map_ents"      },
                                 {CommonAssetType::GLASS_WORLD,          "glass_world"         },
                                 {CommonAssetType::PATH_DATA,            "path_data"           },
                                 {CommonAssetType::VEHICLE_TRACK,        "vehicle_track"       },
                                 {CommonAssetType::ATTACHMENT,           "attachment"          },
                                 {CommonAssetType::SURFACE_FX,           "surface_fx"          },
                                 {CommonAssetType::SCRIPT,               "script"              },
                                 {CommonAssetType::PHYS_CONSTRAINTS,     "phys_constraints"    },
                                 {CommonAssetType::DESTRUCTIBLE_DEF,     "destructible_def"    },
                                 {CommonAssetType::SOUND_PATCH,          "sound_patch"         },
                                 {CommonAssetType::WEAPON_DEF,           "weapon_def"          },
                                 {CommonAssetType::WEAPON_VARIANT,       "weapon_variant"      },
                                 {CommonAssetType::MP_BODY,              "mp_body"             },
                                 {CommonAssetType::MP_HEAD,              "mp_head"             },
                                 {CommonAssetType::PACK_INDEX,           "pack_index"          },
                                 {CommonAssetType::XGLOBALS,             "xglobals"            },
                                 {CommonAssetType::DDL,                  "ddl"                 },
                                 {CommonAssetType::GLASSES,              "glasses"             },
                                 {CommonAssetType::EMBLEM_SET,           "emblem_set"          },
                                 {CommonAssetType::FONT_ICON,            "font_icon"           },
                                 {CommonAssetType::WEAPON_FULL,          "weapon_full"         },
                                 {CommonAssetType::ATTACHMENT_UNIQUE,    "attachment_unique"   },
                                 {CommonAssetType::WEAPON_CAMO,          "weapon_camo"         },
                                 {CommonAssetType::KEY_VALUE_PAIRS,      "key_value_pairs"     },
                                 {CommonAssetType::MEMORY_BLOCK,         "memory_block"        },
                                 {CommonAssetType::SKINNED_VERTS,        "skinned_verts"       },
                                 {CommonAssetType::QDB,                  "qdb"                 },
                                 {CommonAssetType::SLUG,                 "slug"                },
                                 {CommonAssetType::FOOTSTEP_TABLE,       "footstep_table"      },
                                 {CommonAssetType::FOOTSTEP_FX_TABLE,    "footstep_fx_table"   },
                                 {CommonAssetType::ZBARRIER,             "zbarrier"            },
});

namespace
{
    class AssetDto
    {
    public:
        CommonAssetType type;
        std::string name;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(AssetDto, type, name);

    class ZoneAssetsDto
    {
    public:
        std::vector<AssetDto> assets;
        std::vector<AssetDto> references;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(ZoneAssetsDto, assets, references);

    ZoneAssetsDto CreateZoneAssetsDto(const Zone& zone)
    {
        std::vector<AssetDto> assets;
        std::vector<AssetDto> references;

        // Reserve some entries already. Numbers are arbitrary.
        const auto assetCount = zone.m_pools.GetTotalAssetCount();
        assets.reserve(assetCount / 2);
        references.reserve(assetCount / 8);

        const auto& assetTypeMapper = *ICommonAssetTypeMapper::GetCommonAssetMapperByGame(zone.m_game_id);
        for (const auto& asset : zone.m_pools)
        {
            if (asset->IsReference())
            {
                references.emplace_back(AssetDto{
                    .type = assetTypeMapper.GameToCommonAssetType(asset->m_type),
                    .name = asset->ReferencedAssetName(),
                });
            }
            else
            {
                assets.emplace_back(AssetDto{
                    .type = assetTypeMapper.GameToCommonAssetType(asset->m_type),
                    .name = asset->m_name,
                });
            }
        }

        return ZoneAssetsDto{
            .assets = std::move(assets),
            .references = std::move(references),
        };
    }

    std::optional<ZoneAssetsDto> GetZonesForZone(const std::string& zoneName)
    {
        auto& context = ModManContext::Get().m_fast_file;

        ZoneAssetsDto result;

        {
            std::shared_lock lock(context.m_zone_lock);

            for (const auto& loadedZone : context.m_loaded_zones)
            {
                const auto& zone = *loadedZone->m_zone;
                if (zone.m_name == zoneName)
                    return CreateZoneAssetsDto(zone);
            }
        }

        return std::nullopt;
    }
} // namespace

namespace ui
{
    void RegisterAssetBinds(webwindowed::commands_builder& commands)
    {
        Bind<std::string, std::optional<ZoneAssetsDto>>(commands,
                                                        "getAssetsForZone",
                                                        [](webwindowed::detail::window_base& calling_window, const std::string& zoneName)
                                                        {
                                                            return GetZonesForZone(zoneName);
                                                        });
    }
} // namespace ui
