#include "InfoStringLoaderWeaponIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/InfoString/EnumStrings.h"
#include "Game/IW4/InfoString/InfoStringToStructConverter.h"
#include "Game/IW4/Weapon/WeaponFields.h"
#include "Utils/Logging/Log.h"
#include "Weapon/AccuracyGraphLoader.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace IW4;

namespace
{
    class InfoStringToWeaponConverter final : public InfoStringToStructConverter
    {
        bool ConvertHideTags(const cspField_t& field, const std::string& value)
        {
            std::vector<std::string> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                con::error("Failed to parse hide tags as array");
                return false;
            }

            if (valueArray.size() > std::extent_v<decltype(WeaponFullDef::hideTags)>)
            {
                con::error("Cannot have more than {} hide tags!", std::extent_v<decltype(WeaponFullDef::hideTags)>);
                return false;
            }

            auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

            if (valueArray.size() < std::extent_v<decltype(WeaponFullDef::hideTags)>)
            {
                m_registration.AddScriptString(m_zone_script_strings.AddOrGetScriptString(nullptr));
            }

            auto currentHideTag = 0u;
            for (; currentHideTag < valueArray.size(); currentHideTag++)
            {
                const auto& currentValue = valueArray[currentHideTag];
                const auto scrString =
                    !currentValue.empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue) : m_zone_script_strings.AddOrGetScriptString(nullptr);
                hideTags[currentHideTag] = scrString;
                m_registration.AddScriptString(scrString);
            }

            for (; currentHideTag < std::extent_v<decltype(WeaponFullDef::hideTags)>; currentHideTag++)
            {
                hideTags[currentHideTag] = m_zone_script_strings.GetScriptString(nullptr);
            }

            return true;
        }

        _NODISCARD bool ConvertBounceSounds(const cspField_t& field, const std::string& value) const
        {
            auto** bounceSound = reinterpret_cast<SndAliasCustom**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            if (value.empty())
            {
                *bounceSound = nullptr;
                return true;
            }

            assert(std::extent_v<decltype(bounceSoundSuffixes)> == SURF_TYPE_NUM);
            *bounceSound = m_memory.Alloc<SndAliasCustom>(SURF_TYPE_NUM);
            for (auto i = 0u; i < SURF_TYPE_NUM; i++)
            {
                const auto currentBounceSound = value + bounceSoundSuffixes[i];

                (*bounceSound)[i].name = m_memory.Alloc<snd_alias_list_name>();
                (*bounceSound)[i].name->soundName = m_memory.Dup(currentBounceSound.c_str());
            }

            return true;
        }

        _NODISCARD bool ConvertNotetrackMap(const cspField_t& field, const std::string& value, const char* mapName, const size_t keyAndValueCount)
        {
            std::vector<std::array<std::string, 2>> pairs;
            if (!ParseAsArray(value, pairs))
            {
                con::error("Failed to parse notetrack{}map as pairs", mapName);
                return false;
            }

            if (pairs.size() > std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>)
            {
                con::error("Cannot have more than {} notetrack{}map entries!", std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>, mapName);
                return false;
            }

            auto* keys = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            auto* values = &keys[keyAndValueCount];
            auto currentEntryNum = 0u;

            if (pairs.size() < keyAndValueCount)
            {
                m_registration.AddScriptString(m_zone_script_strings.AddOrGetScriptString(nullptr));
            }

            for (; currentEntryNum < pairs.size(); currentEntryNum++)
            {
                const auto& currentValue = pairs[currentEntryNum];
                const auto keyScriptString = !currentValue[0].empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue[0])
                                                                      : m_zone_script_strings.AddOrGetScriptString(nullptr);
                const auto valueScriptString = !currentValue[1].empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue[1])
                                                                        : m_zone_script_strings.AddOrGetScriptString(nullptr);

                keys[currentEntryNum] = keyScriptString;
                m_registration.AddScriptString(keyScriptString);

                values[currentEntryNum] = valueScriptString;
                m_registration.AddScriptString(valueScriptString);
            }

            for (; currentEntryNum < keyAndValueCount; currentEntryNum++)
            {
                const auto emptyScr = m_zone_script_strings.GetScriptString(nullptr);
                keys[currentEntryNum] = emptyScr;
                values[currentEntryNum] = emptyScr;
            }

            return true;
        }

        bool ConvertAnimName(const cspField_t& field, const std::string& value)
        {
            if (ConvertString(value, field.iOffset))
            {
                if (!value.empty())
                    m_registration.AddIndirectAssetReference(m_context.LoadIndirectAssetReference<AssetXAnim>(value));

                return true;
            }

            return false;
        }

    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<weapFieldType_t>(field.iFieldType))
            {
            case WFT_WEAPONTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapTypeNames, std::extent_v<decltype(szWeapTypeNames)>);

            case WFT_WEAPONCLASS:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapClassNames, std::extent_v<decltype(szWeapClassNames)>);

            case WFT_OVERLAYRETICLE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapOverlayReticleNames, std::extent_v<decltype(szWeapOverlayReticleNames)>);

            case WFT_PENETRATE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, penetrateTypeNames, std::extent_v<decltype(penetrateTypeNames)>);

            case WFT_IMPACT_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, impactTypeNames, std::extent_v<decltype(impactTypeNames)>);

            case WFT_STANCE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapStanceNames, std::extent_v<decltype(szWeapStanceNames)>);

            case WFT_PROJ_EXPLOSION:
                return ConvertEnumInt(field.szName, value, field.iOffset, szProjectileExplosionNames, std::extent_v<decltype(szProjectileExplosionNames)>);

            case WFT_OFFHAND_CLASS:
                return ConvertEnumInt(field.szName, value, field.iOffset, offhandClassNames, std::extent_v<decltype(offhandClassNames)>);

            case WFT_ANIMTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, playerAnimTypeNames, std::extent_v<decltype(playerAnimTypeNames)>);

            case WFT_ACTIVE_RETICLE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, activeReticleNames, std::extent_v<decltype(activeReticleNames)>);

            case WFT_GUIDED_MISSILE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, guidedMissileNames, std::extent_v<decltype(guidedMissileNames)>);

            case WFT_BOUNCE_SOUND:
                return ConvertBounceSounds(field, value);

            case WFT_STICKINESS:
                return ConvertEnumInt(field.szName, value, field.iOffset, stickinessNames, std::extent_v<decltype(stickinessNames)>);

            case WFT_OVERLAYINTERFACE:
                return ConvertEnumInt(field.szName, value, field.iOffset, overlayInterfaceNames, std::extent_v<decltype(overlayInterfaceNames)>);

            case WFT_INVENTORYTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapInventoryTypeNames, std::extent_v<decltype(szWeapInventoryTypeNames)>);

            case WFT_FIRETYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapFireTypeNames, std::extent_v<decltype(szWeapFireTypeNames)>);

            case WFT_AMMOCOUNTER_CLIPTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, ammoCounterClipNames, std::extent_v<decltype(ammoCounterClipNames)>);

            case WFT_ICONRATIO_HUD:
            case WFT_ICONRATIO_PICKUP:
            case WFT_ICONRATIO_AMMOCOUNTER:
            case WFT_ICONRATIO_KILL:
            case WFT_ICONRATIO_DPAD:
                return ConvertEnumInt(field.szName, value, field.iOffset, weapIconRatioNames, std::extent_v<decltype(weapIconRatioNames)>);

            case WFT_HIDETAGS:
                return ConvertHideTags(field, value);

            case WFT_NOTETRACKSOUNDMAP:
                return ConvertNotetrackMap(field, value, "sound", std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>);

            case WFT_NOTETRACKRUMBLEMAP:
                return ConvertNotetrackMap(field, value, "rumble", std::extent_v<decltype(WeaponFullDef::notetrackRumbleMapKeys)>);

            case WFT_ANIM_NAME:
                return ConvertAnimName(field, value);

            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToWeaponConverter(const InfoString& infoString,
                                    WeaponFullDef& weaponFullDef,
                                    ZoneScriptStrings& zoneScriptStrings,
                                    MemoryManager& memory,
                                    AssetCreationContext& context,
                                    AssetRegistration<AssetWeapon>& registration,
                                    const cspField_t* fields,
                                    const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &weaponFullDef, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };

    void InitWeaponFullDef(WeaponFullDef& weapon)
    {
        weapon.weapCompleteDef.weapDef = &weapon.weapDef;
        weapon.weapCompleteDef.hideTags = weapon.hideTags;
        weapon.weapCompleteDef.szXAnims = weapon.szXAnims;
        weapon.weapDef.gunXModel = weapon.gunXModel;
        weapon.weapDef.szXAnimsRightHanded = weapon.szXAnimsRightHanded;
        weapon.weapDef.szXAnimsLeftHanded = weapon.szXAnimsLeftHanded;
        weapon.weapDef.notetrackSoundMapKeys = weapon.notetrackSoundMapKeys;
        weapon.weapDef.notetrackSoundMapValues = weapon.notetrackSoundMapValues;
        weapon.weapDef.notetrackRumbleMapKeys = weapon.notetrackRumbleMapKeys;
        weapon.weapDef.notetrackRumbleMapValues = weapon.notetrackRumbleMapValues;
        weapon.weapDef.worldModel = weapon.worldModel;
        weapon.weapDef.parallelBounce = weapon.parallelBounce;
        weapon.weapDef.perpendicularBounce = weapon.perpendicularBounce;
        weapon.weapDef.locationDamageMultipliers = weapon.locationDamageMultipliers;

        for (const auto& field : weapon_fields)
        {
            if (field.iFieldType != CSPFT_STRING)
                continue;

            *reinterpret_cast<const char**>(reinterpret_cast<char*>(&weapon) + field.iOffset) = "";
        }
    }

    void CheckProjectileValues(const WeaponCompleteDef& weaponCompleteDef, const WeaponDef& weaponDef)
    {
        if (weaponDef.iProjectileSpeed <= 0)
            con::error("Projectile speed for WeapType {} must be greater than 0.0", weaponCompleteDef.szDisplayName);
        if (weaponDef.destabilizationCurvatureMax >= 1000000000.0f || weaponDef.destabilizationCurvatureMax < 0.0f)
            con::error("Destabilization angle for for WeapType {} must be between 0 and 45 degrees", weaponCompleteDef.szDisplayName);
        if (weaponDef.destabilizationRateTime < 0.0f)
            con::error("Destabilization rate time for for WeapType {} must be non-negative", weaponCompleteDef.szDisplayName);
    }

    void CheckTurretBarrelSpin(const WeaponCompleteDef& weaponCompleteDef, const WeaponDef& weaponDef)
    {
        if (weaponDef.weapClass != WEAPCLASS_TURRET)
            con::error("Rotating barrel set for non-turret weapon {}.", weaponCompleteDef.szInternalName);

        if (0.0f == weaponDef.turretBarrelSpinSpeed)
        {
            con::error("Rotating barrel spin speed '{}' is invalid for weapon {}.", weaponDef.turretBarrelSpinSpeed, weaponCompleteDef.szInternalName);
        }
        if (0.0f < weaponDef.turretOverheatUpRate && 0.0f >= weaponDef.turretOverheatDownRate)
        {
            con::error("Turret overheat Up rate is set, but the down rate '{}' is invalid for weapon {}.",
                       weaponDef.turretOverheatDownRate,
                       weaponCompleteDef.szInternalName);
        }
    }

    void CheckThermalScope(const WeaponCompleteDef& weaponCompleteDef, const WeaponDef& weaponDef)
    {
        if (0.0f != weaponDef.fAdsZoomInFrac)
        {
            con::error("Weapon {} ({}) has thermal scope set. ADS Zoom In frac should be 0 to prevent zoom-in blur ({}).",
                       weaponCompleteDef.szInternalName,
                       weaponCompleteDef.szDisplayName,
                       weaponDef.fAdsZoomInFrac);
        }

        if (0.0f != weaponDef.fAdsZoomOutFrac)
        {
            con::error("Weapon {} ({}) has thermal scope set. ADS Zoom Out frac should be 0 to prevent zoom-out blur ({}).",
                       weaponCompleteDef.szInternalName,
                       weaponCompleteDef.szDisplayName,
                       weaponDef.fAdsZoomOutFrac);
        }
    }

    void CalculateWeaponFields(WeaponFullDef& weapon, MemoryManager& memory)
    {
        auto& weaponCompleteDef = weapon.weapCompleteDef;
        auto& weaponDef = weapon.weapDef;

        if (!weaponDef.viewLastShotEjectEffect)
            weaponDef.viewLastShotEjectEffect = weaponDef.viewShellEjectEffect;
        if (!weaponDef.worldLastShotEjectEffect)
            weaponDef.worldLastShotEjectEffect = weaponDef.worldShellEjectEffect;

        if (weaponCompleteDef.iAdsTransInTime <= 0)
            weaponDef.fOOPosAnimLength[0] = 0.0033333334f;
        else
            weaponDef.fOOPosAnimLength[0] = 1.0f / static_cast<float>(weaponCompleteDef.iAdsTransInTime);

        if (weaponCompleteDef.iAdsTransOutTime <= 0)
            weaponDef.fOOPosAnimLength[1] = 0.0020000001f;
        else
            weaponDef.fOOPosAnimLength[1] = 1.0f / static_cast<float>(weaponCompleteDef.iAdsTransOutTime);

        if (weaponDef.fMaxDamageRange < 0.0f)
            weaponDef.fMaxDamageRange = 999999.0f;
        if (weaponDef.fMinDamageRange < 0.0f)
            weaponDef.fMinDamageRange = 999999.12f;

        if (weaponDef.enemyCrosshairRange > 15000.0f)
            con::error("Enemy crosshair ranges should be less than {}", 15000.0f);

        if (weaponDef.weapType == WEAPTYPE_PROJECTILE)
            CheckProjectileValues(weaponCompleteDef, weaponDef);

        if (weaponDef.turretBarrelSpinEnabled)
            CheckTurretBarrelSpin(weaponCompleteDef, weaponDef);

        if (weaponDef.thermalScope)
            CheckThermalScope(weaponCompleteDef, weaponDef);

        if (weaponDef.offhandClass && !weaponDef.bClipOnly)
        {
            con::error("Weapon {} ({}) is an offhand weapon but is not set to clip only, which is not supported since we can't reload the offhand slot.",
                       weaponCompleteDef.szInternalName,
                       weaponCompleteDef.szDisplayName);
        }

        if (weaponDef.weapType == WEAPTYPE_BULLET)
        {
            if (weaponDef.bulletExplDmgMult <= 0.0f)
                con::error("Detected invalid bulletExplDmgMult of '{}' for weapon '{}'; please update weapon settings.",
                           weaponDef.bulletExplDmgMult,
                           weaponCompleteDef.szInternalName);
            if (weaponDef.bulletExplRadiusMult <= 0.0f)
                con::error("Detected invalid bulletExplRadiusMult of '{}' for weapon '{}'; please update weapon settings.",
                           weaponDef.bulletExplRadiusMult,
                           weaponCompleteDef.szInternalName);
        }
    }

    void ConvertAccuracyGraph(const GenericGraph2D& graph,
                              vec2_t*& originalGraphKnots,
                              uint16_t& originalGraphKnotCount,
                              vec2_t*& graphKnots,
                              uint16_t& graphKnotCount,
                              MemoryManager& memory)
    {
        originalGraphKnotCount = static_cast<uint16_t>(graph.knots.size());
        originalGraphKnots = memory.Alloc<vec2_t>(originalGraphKnotCount);

        for (auto i = 0u; i < originalGraphKnotCount; i++)
        {
            const auto& commonKnot = graph.knots[i];
            originalGraphKnots[i].x = static_cast<float>(commonKnot.x);
            originalGraphKnots[i].y = static_cast<float>(commonKnot.y);
        }

        graphKnots = originalGraphKnots;
        graphKnotCount = originalGraphKnotCount;
    }

    bool LoadAccuracyGraphs(WeaponFullDef& weaponFullDef, MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
    {
        auto& accuracyGraphLoader = context.GetZoneAssetCreationState<AccuracyGraphLoader>();

        if (weaponFullDef.weapDef.aiVsAiAccuracyGraphName && weaponFullDef.weapDef.aiVsAiAccuracyGraphName[0])
        {
            const auto* graph = accuracyGraphLoader.LoadAiVsAiGraph(searchPath, weaponFullDef.weapDef.aiVsAiAccuracyGraphName);
            if (!graph)
                return false;

            ConvertAccuracyGraph(*graph,
                                 weaponFullDef.weapDef.originalAiVsAiAccuracyGraphKnots,
                                 weaponFullDef.weapDef.originalAiVsAiAccuracyGraphKnotCount,
                                 weaponFullDef.weapCompleteDef.aiVsAiAccuracyGraphKnots,
                                 weaponFullDef.weapCompleteDef.aiVsAiAccuracyGraphKnotCount,
                                 memory);
        }

        if (weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName && weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName[0])
        {
            const auto* graph = accuracyGraphLoader.LoadAiVsPlayerGraph(searchPath, weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName);
            if (!graph)
                return false;

            ConvertAccuracyGraph(*graph,
                                 weaponFullDef.weapDef.originalAiVsPlayerAccuracyGraphKnots,
                                 weaponFullDef.weapDef.originalAiVsPlayerAccuracyGraphKnotCount,
                                 weaponFullDef.weapCompleteDef.aiVsPlayerAccuracyGraphKnots,
                                 weaponFullDef.weapCompleteDef.aiVsPlayerAccuracyGraphKnotCount,
                                 memory);
        }

        return true;
    }
} // namespace

namespace weapon
{
    InfoStringLoaderIW4::InfoStringLoaderIW4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
        : m_memory(memory),
          m_search_path(searchPath),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoaderIW4::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        auto* weaponFullDef = m_memory.Alloc<WeaponFullDef>();

        InitWeaponFullDef(*weaponFullDef);
        weaponFullDef->weapCompleteDef.szInternalName = m_memory.Dup(assetName.c_str());

        AssetRegistration<AssetWeapon> registration(assetName, &weaponFullDef->weapCompleteDef);

        InfoStringToWeaponConverter converter(
            infoString, *weaponFullDef, m_zone.m_script_strings, m_memory, context, registration, weapon_fields, std::extent_v<decltype(weapon_fields)>);
        if (!converter.Convert())
        {
            con::error("Failed to parse weapon: \"{}\"", assetName);
            return AssetCreationResult::Failure();
        }

        CalculateWeaponFields(*weaponFullDef, m_memory);

        LoadAccuracyGraphs(*weaponFullDef, m_memory, m_search_path, context);

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace weapon
