#include "WeaponDumperIW3.h"

#include "Game/IW3/CommonIW3.h"
#include "Game/IW3/InfoString/InfoStringFromStructConverter.h"
#include "Game/IW3/ObjConstantsIW3.h"
#include "Game/IW3/Weapon/WeaponFields.h"
#include "Game/IW3/Weapon/WeaponStrings.h"
#include "Weapon/AccuracyGraphWriter.h"
#include "Weapon/WeaponCommon.h"

#include <cassert>
#include <functional>
#include <sstream>
#include <type_traits>
#include <utility>

using namespace IW3;

namespace
{
    class InfoStringFromWeaponConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<weapFieldType_t>(field.iFieldType))
            {
            case WFT_WEAPONTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapTypeNames, std::extent_v<decltype(szWeapTypeNames)>);
                break;

            case WFT_WEAPONCLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapClassNames, std::extent_v<decltype(szWeapClassNames)>);
                break;

            case WFT_OVERLAYRETICLE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapOverlayReticleNames, std::extent_v<decltype(szWeapOverlayReticleNames)>);
                break;

            case WFT_PENETRATE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, penetrateTypeNames, std::extent_v<decltype(penetrateTypeNames)>);
                break;

            case WFT_IMPACT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, impactTypeNames, std::extent_v<decltype(impactTypeNames)>);
                break;

            case WFT_STANCE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapStanceNames, std::extent_v<decltype(szWeapStanceNames)>);
                break;

            case WFT_PROJ_EXPLOSION:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szProjectileExplosionNames, std::extent_v<decltype(szProjectileExplosionNames)>);
                break;

            case WFT_OFFHAND_CLASS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, offhandClassNames, std::extent_v<decltype(offhandClassNames)>);
                break;

            case WFT_ANIMTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, playerAnimTypeNames, std::extent_v<decltype(playerAnimTypeNames)>);
                break;

            case WFT_ACTIVE_RETICLE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, activeReticleNames, std::extent_v<decltype(activeReticleNames)>);
                break;

            case WFT_GUIDED_MISSILE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, guidedMissileNames, std::extent_v<decltype(guidedMissileNames)>);
                break;

            case WFT_BOUNCE_SOUND:
            {
                const auto* bounceSound = *reinterpret_cast<SndAliasCustom**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

                if (bounceSound && bounceSound->name)
                {
                    const std::string firstBounceSound(bounceSound->name->soundName);
                    const auto endOfBouncePrefix = firstBounceSound.rfind("_default");
                    assert(endOfBouncePrefix != std::string::npos);

                    if (endOfBouncePrefix != std::string::npos)
                    {
                        m_info_string.SetValueForKey(std::string(field.szName), firstBounceSound.substr(0, endOfBouncePrefix));
                    }
                    else
                        m_info_string.SetValueForKey(std::string(field.szName), "");
                }
                else
                    m_info_string.SetValueForKey(std::string(field.szName), "");

                break;
            }

            case WFT_STICKINESS:
                FillFromEnumInt(std::string(field.szName), field.iOffset, stickinessNames, std::extent_v<decltype(stickinessNames)>);
                break;

            case WFT_OVERLAYINTERFACE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, overlayInterfaceNames, std::extent_v<decltype(overlayInterfaceNames)>);
                break;

            case WFT_INVENTORYTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapInventoryTypeNames, std::extent_v<decltype(szWeapInventoryTypeNames)>);
                break;

            case WFT_FIRETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapFireTypeNames, std::extent_v<decltype(szWeapFireTypeNames)>);
                break;

            case WFT_AMMOCOUNTER_CLIPTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, ammoCounterClipNames, std::extent_v<decltype(ammoCounterClipNames)>);
                break;

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
                FillFromEnumInt(std::string(field.szName), field.iOffset, weapIconRatioNames, std::extent_v<decltype(weapIconRatioNames)>);
                break;

            case WFT_HIDETAGS:
            {
                const auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                std::stringstream ss;
                bool first = true;

                for (auto i = 0u; i < std::extent_v<decltype(WeaponDef::hideTags)>; i++)
                {
                    const auto& str = m_get_scr_string(hideTags[i]);
                    if (!str.empty())
                    {
                        if (!first)
                            ss << "\n";
                        else
                            first = false;

                        ss << str;
                    }
                }

                m_info_string.SetValueForKey(std::string(field.szName), ss.str());
                break;
            }

            case WFT_NOTETRACKSOUNDMAP:
            {
                const auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
                const auto* values = &keys[std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>];
                std::stringstream ss;
                bool first = true;

                for (auto i = 0u; i < std::extent_v<decltype(WeaponDef::notetrackSoundMapKeys)>; i++)
                {
                    const auto& key = m_get_scr_string(keys[i]);
                    const auto& value = m_get_scr_string(values[i]);
                    if (!key.empty())
                    {
                        if (!first)
                            ss << "\n";
                        else
                            first = false;

                        ss << key;

                        if (!value.empty())
                            ss << " " << value;
                    }
                }

                m_info_string.SetValueForKey(std::string(field.szName), ss.str());
                break;
            }

            case WFT_ANIM_NAME:
                FillFromString(std::string(field.szName), field.iOffset);
                break;

            case WFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromWeaponConverter(const WeaponDef* structure,
                                      const cspField_t* fields,
                                      const size_t fieldCount,
                                      std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };

    InfoString CreateInfoString(const XAssetInfo<WeaponDef>& asset)
    {
        InfoStringFromWeaponConverter converter(asset.Asset(),
                                                weapon_fields,
                                                std::extent_v<decltype(weapon_fields)>,
                                                [asset](const scr_string_t scrStr) -> std::string
                                                {
                                                    assert(scrStr < asset.m_zone->m_script_strings.Count());
                                                    if (scrStr >= asset.m_zone->m_script_strings.Count())
                                                        return "";

                                                    return asset.m_zone->m_script_strings[scrStr];
                                                });

        return converter.Convert();
    }

    GenericGraph2D ConvertAccuracyGraph(std::string graphName, const vec2_t* originalKnots, const unsigned originalKnotCount)
    {
        GenericGraph2D graph;

        graph.name = std::move(graphName);
        graph.knots.resize(originalKnotCount);

        for (auto i = 0u; i < originalKnotCount; i++)
        {
            auto& knot = graph.knots[i];
            knot.x = originalKnots[i].x;
            knot.y = originalKnots[i].y;
        }

        return graph;
    }

    void DumpAccuracyGraphs(AssetDumpingContext& context, const XAssetInfo<WeaponDef>& asset)
    {
        auto* accuracyGraphWriter = context.GetZoneAssetDumperState<AccuracyGraphWriter>();
        const auto* weapon = asset.Asset();

        if (weapon->aiVsAiAccuracyGraphName && weapon->originalAiVsAiAccuracyGraphKnots)
        {
            auto graphName = weapon::GetAssetNameForAiVsAiAccuracyGraph(weapon->aiVsAiAccuracyGraphName);
            if (accuracyGraphWriter->ShouldDumpGraph(graphName))
            {
                const auto graph =
                    ConvertAccuracyGraph(std::move(graphName), weapon->originalAiVsAiAccuracyGraphKnots, weapon->originalAiVsAiAccuracyGraphKnotCount);
                AccuracyGraphWriter::DumpGraph(context, graph);
            }
        }

        if (weapon->aiVsPlayerAccuracyGraphName && weapon->originalAiVsPlayerAccuracyGraphKnots)
        {
            auto graphName = weapon::GetAssetNameForAiVsPlayerAccuracyGraph(weapon->aiVsPlayerAccuracyGraphName);
            if (accuracyGraphWriter->ShouldDumpGraph(graphName))
            {
                const auto graph =
                    ConvertAccuracyGraph(std::move(graphName), weapon->originalAiVsPlayerAccuracyGraphKnots, weapon->originalAiVsPlayerAccuracyGraphKnotCount);
                AccuracyGraphWriter::DumpGraph(context, graph);
            }
        }
    }
} // namespace

namespace weapon
{
    void DumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetWeapon::Type>& asset)
    {
        // Only dump raw when no gdt available
        if (context.m_gdt)
        {
            const auto infoString = CreateInfoString(asset);
            GdtEntry gdtEntry(asset.m_name, GDF_FILENAME_WEAPON);
            infoString.ToGdtProperties(INFO_STRING_PREFIX_WEAPON, gdtEntry);
            context.m_gdt->WriteEntry(gdtEntry);
        }
        else
        {
            const auto assetFile = context.OpenAssetFile(GetFileNameForAssetName(asset.m_name));

            if (!assetFile)
                return;

            auto& stream = *assetFile;
            const auto infoString = CreateInfoString(asset);
            const auto stringValue = infoString.ToString(INFO_STRING_PREFIX_WEAPON);
            stream.write(stringValue.c_str(), stringValue.size());
        }

        DumpAccuracyGraphs(context, asset);
    }
} // namespace weapon
