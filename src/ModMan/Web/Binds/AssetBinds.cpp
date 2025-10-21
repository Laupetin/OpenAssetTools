#include "AssetBinds.h"

#include "Context/ModManContext.h"
#include "Game/CommonAsset.h"
#include "Web/UiCommunication.h"

#include "Json/JsonExtension.h"
#include <cstdint>

NLOHMANN_JSON_SERIALIZE_ENUM(CommonAssetType,
                             {
                                 {CommonAssetType::PHYS_PRESET,          "PHYS_PRESET"         },
                                 {CommonAssetType::XANIM,                "XANIM"               },
                                 {CommonAssetType::XMODEL,               "XMODEL"              },
                                 {CommonAssetType::MATERIAL,             "MATERIAL"            },
                                 {CommonAssetType::TECHNIQUE_SET,        "TECHNIQUE_SET"       },
                                 {CommonAssetType::IMAGE,                "IMAGE"               },
                                 {CommonAssetType::SOUND,                "SOUND"               },
                                 {CommonAssetType::SOUND_CURVE,          "SOUND_CURVE"         },
                                 {CommonAssetType::LOADED_SOUND,         "LOADED_SOUND"        },
                                 {CommonAssetType::CLIP_MAP,             "CLIP_MAP"            },
                                 {CommonAssetType::COM_WORLD,            "COM_WORLD"           },
                                 {CommonAssetType::GAME_WORLD_SP,        "GAME_WORLD_SP"       },
                                 {CommonAssetType::GAME_WORLD_MP,        "GAME_WORLD_MP"       },
                                 {CommonAssetType::MAP_ENTS,             "MAP_ENTS"            },
                                 {CommonAssetType::GFX_WORLD,            "GFX_WORLD"           },
                                 {CommonAssetType::LIGHT_DEF,            "LIGHT_DEF"           },
                                 {CommonAssetType::UI_MAP,               "UI_MAP"              },
                                 {CommonAssetType::FONT,                 "FONT"                },
                                 {CommonAssetType::MENULIST,             "MENULIST"            },
                                 {CommonAssetType::MENU,                 "MENU"                },
                                 {CommonAssetType::LOCALIZE_ENTRY,       "LOCALIZE_ENTRY"      },
                                 {CommonAssetType::WEAPON,               "WEAPON"              },
                                 {CommonAssetType::SOUND_DRIVER_GLOBALS, "SOUND_DRIVER_GLOBALS"},
                                 {CommonAssetType::FX,                   "FX"                  },
                                 {CommonAssetType::IMPACT_FX,            "IMPACT_FX"           },
                                 {CommonAssetType::AI_TYPE,              "AI_TYPE"             },
                                 {CommonAssetType::MP_TYPE,              "MP_TYPE"             },
                                 {CommonAssetType::CHARACTER,            "CHARACTER"           },
                                 {CommonAssetType::XMODEL_ALIAS,         "XMODEL_ALIAS"        },
                                 {CommonAssetType::RAW_FILE,             "RAW_FILE"            },
                                 {CommonAssetType::STRING_TABLE,         "STRING_TABLE"        },
                                 {CommonAssetType::XMODEL_PIECES,        "XMODEL_PIECES"       },
                                 {CommonAssetType::PHYS_COLL_MAP,        "PHYS_COLL_MAP"       },
                                 {CommonAssetType::XMODEL_SURFS,         "XMODEL_SURFS"        },
                                 {CommonAssetType::PIXEL_SHADER,         "PIXEL_SHADER"        },
                                 {CommonAssetType::VERTEX_SHADER,        "VERTEX_SHADER"       },
                                 {CommonAssetType::VERTEX_DECL,          "VERTEX_DECL"         },
                                 {CommonAssetType::FX_WORLD,             "FX_WORLD"            },
                                 {CommonAssetType::LEADERBOARD,          "LEADERBOARD"         },
                                 {CommonAssetType::STRUCTURED_DATA_DEF,  "STRUCTURED_DATA_DEF" },
                                 {CommonAssetType::TRACER,               "TRACER"              },
                                 {CommonAssetType::VEHICLE,              "VEHICLE"             },
                                 {CommonAssetType::ADDON_MAP_ENTS,       "ADDON_MAP_ENTS"      },
                                 {CommonAssetType::GLASS_WORLD,          "GLASS_WORLD"         },
                                 {CommonAssetType::PATH_DATA,            "PATH_DATA"           },
                                 {CommonAssetType::VEHICLE_TRACK,        "VEHICLE_TRACK"       },
                                 {CommonAssetType::ATTACHMENT,           "ATTACHMENT"          },
                                 {CommonAssetType::SURFACE_FX,           "SURFACE_FX"          },
                                 {CommonAssetType::SCRIPT,               "SCRIPT"              },
                                 {CommonAssetType::PHYS_CONSTRAINTS,     "PHYS_CONSTRAINTS"    },
                                 {CommonAssetType::DESTRUCTIBLE_DEF,     "DESTRUCTIBLE_DEF"    },
                                 {CommonAssetType::SOUND_PATCH,          "SOUND_PATCH"         },
                                 {CommonAssetType::WEAPON_DEF,           "WEAPON_DEF"          },
                                 {CommonAssetType::WEAPON_VARIANT,       "WEAPON_VARIANT"      },
                                 {CommonAssetType::MP_BODY,              "MP_BODY"             },
                                 {CommonAssetType::MP_HEAD,              "MP_HEAD"             },
                                 {CommonAssetType::PACK_INDEX,           "PACK_INDEX"          },
                                 {CommonAssetType::XGLOBALS,             "XGLOBALS"            },
                                 {CommonAssetType::DDL,                  "DDL"                 },
                                 {CommonAssetType::GLASSES,              "GLASSES"             },
                                 {CommonAssetType::EMBLEM_SET,           "EMBLEM_SET"          },
                                 {CommonAssetType::FONT_ICON,            "FONT_ICON"           },
                                 {CommonAssetType::WEAPON_FULL,          "WEAPON_FULL"         },
                                 {CommonAssetType::ATTACHMENT_UNIQUE,    "ATTACHMENT_UNIQUE"   },
                                 {CommonAssetType::WEAPON_CAMO,          "WEAPON_CAMO"         },
                                 {CommonAssetType::KEY_VALUE_PAIRS,      "KEY_VALUE_PAIRS"     },
                                 {CommonAssetType::MEMORY_BLOCK,         "MEMORY_BLOCK"        },
                                 {CommonAssetType::SKINNED_VERTS,        "SKINNED_VERTS"       },
                                 {CommonAssetType::QDB,                  "QDB"                 },
                                 {CommonAssetType::SLUG,                 "SLUG"                },
                                 {CommonAssetType::FOOTSTEP_TABLE,       "FOOTSTEP_TABLE"      },
                                 {CommonAssetType::FOOTSTEP_FX_TABLE,    "FOOTSTEP_FX_TABLE"   },
                                 {CommonAssetType::ZBARRIER,             "ZBARRIER"            },
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
        const auto assetCount = zone.m_pools->GetTotalAssetCount();
        assets.reserve(assetCount / 2);
        references.reserve(assetCount / 8);

        const auto& assetTypeMapper = *ICommonAssetTypeMapper::GetCommonAssetMapperByGame(zone.m_game_id);
        for (const auto& asset : *zone.m_pools)
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
    void RegisterAssetBinds(webview::webview& wv)
    {
        Bind<std::string, std::optional<ZoneAssetsDto>>(wv,
                                                        "getAssetsForZone",
                                                        [](const std::string& zoneName)
                                                        {
                                                            return GetZonesForZone(zoneName);
                                                        });
    }
} // namespace ui
