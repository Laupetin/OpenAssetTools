#include "InfoStringLoaderWeaponIW5.h"

#include "Game/IW5/IW5.h"
#include "Game/IW5/InfoString/InfoStringToStructConverter.h"
#include "Game/IW5/Weapon/WeaponFields.h"
#include "Weapon/AccuracyGraphLoader.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace IW5;

namespace
{
    class InfoStringToWeaponConverter final : public InfoStringToStructConverter
    {
        bool ConvertHideTags(const cspField_t& field, const std::string& value)
        {
            std::vector<std::string> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cerr << "Failed to parse hide tags as array\n";
                return false;
            }

            if (valueArray.size() > std::extent_v<decltype(WeaponFullDef::hideTags)>)
            {
                std::cerr << std::format("Cannot have more than {} hide tags!\n", std::extent_v<decltype(WeaponFullDef::hideTags)>);
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

        [[nodiscard]] bool ConvertPerSurfaceTypeSound(const cspField_t& field, const std::string& value) const
        {
            auto** perSurfaceTypeSound = reinterpret_cast<SndAliasCustom**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            if (value.empty())
            {
                *perSurfaceTypeSound = nullptr;
                return true;
            }

            *perSurfaceTypeSound = m_memory.Alloc<SndAliasCustom>(SURF_TYPE_COUNT);
            for (auto i = 0u; i < SURF_TYPE_COUNT; i++)
            {
                const auto currentPerSurfaceTypeSound = value + surfaceTypeSoundSuffixes[i];

                (*perSurfaceTypeSound)[i].name = m_memory.Alloc<snd_alias_list_name>();
                (*perSurfaceTypeSound)[i].name->soundName = m_memory.Dup(currentPerSurfaceTypeSound.c_str());
            }

            return true;
        }

        [[nodiscard]] bool ConvertNoteTrackMap(const cspField_t& field, const std::string& value, const char* mapName, const size_t keyAndValueCount)
        {
            std::vector<std::array<std::string, 2>> pairs;
            if (!ParseAsArray(value, pairs))
            {
                std::cerr << std::format("Failed to parse notetrack{}map as pairs\n", mapName);
                return false;
            }

            if (pairs.size() > std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>)
            {
                std::cerr << std::format(
                    "Cannot have more than {} notetrack{}map entries!\n", std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>, mapName);
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

        bool ConvertAttachments(const std::string& value)
        {
            std::vector<std::string> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cerr << "Failed to parse attachments as array\n";
                return false;
            }

            auto currentScope = 0u;
            auto currentUnderBarrel = 0u;
            auto currentOther = 0u;
            for (const auto& attachmentName : valueArray)
            {
                auto* attachmentInfo = m_context.LoadDependency<AssetAttachment>(attachmentName);
                if (!attachmentInfo)
                    return false;
                m_registration.AddDependency(attachmentInfo);
                auto* attachment = attachmentInfo->Asset();

                if (attachment->type == ATTACHMENT_SCOPE)
                {
                    if (currentScope >= std::extent_v<decltype(WeaponFullDef::scopes)>)
                    {
                        std::cerr << std::format("Cannot have more than {} scopes\n", std::extent_v<decltype(WeaponFullDef::scopes)>);
                        return false;
                    }

                    m_weapon.scopes[currentScope++] = attachment;
                }
                else if (attachment->type == ATTACHMENT_UNDERBARREL)
                {
                    if (currentUnderBarrel >= std::extent_v<decltype(WeaponFullDef::underBarrels)>)
                    {
                        std::cerr << std::format("Cannot have more than {} under barrels\n", std::extent_v<decltype(WeaponFullDef::underBarrels)>);
                        return false;
                    }

                    m_weapon.underBarrels[currentUnderBarrel++] = attachment;
                }
                else if (attachment->type == ATTACHMENT_OTHER)
                {
                    if (currentOther >= std::extent_v<decltype(WeaponFullDef::others)>)
                    {
                        std::cerr << std::format("Cannot have more than {} other attachments\n", std::extent_v<decltype(WeaponFullDef::others)>);
                        return false;
                    }

                    m_weapon.others[currentOther++] = attachment;
                }
            }

            return true;
        }

        bool ConvertAnimOverrides(const std::string& value)
        {
            std::vector<std::array<std::string, 7>> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cerr << "Failed to parse anim overrides as array\n";
                return false;
            }

            auto* animOverrides = m_memory.Alloc<AnimOverrideEntry>(valueArray.size());

            auto i = 0u;
            for (const auto& overrideValues : valueArray)
            {
                auto& animOverride = animOverrides[i++];

                if (!ParseSingleWeaponAttachment(overrideValues[0], animOverride.attachment1))
                    return false;

                if (!ParseSingleWeaponAttachment(overrideValues[1], animOverride.attachment2))
                    return false;

                if (!ParseAnimFile(overrideValues[2], animOverride.animTreeType))
                    return false;

                ParseAnim(overrideValues[3], animOverride.overrideAnim);
                ParseAnim(overrideValues[4], animOverride.altmodeAnim);

                if (!ParseInt(overrideValues[5], animOverride.animTime))
                    return false;

                if (!ParseInt(overrideValues[6], animOverride.altTime))
                    return false;
            }

            m_weapon.weapCompleteDef.animOverrides = animOverrides;
            m_weapon.weapCompleteDef.numAnimOverrides = valueArray.size();

            return true;
        }

        bool ConvertSoundOverrides(const std::string& value)
        {
            std::vector<std::array<std::string, 5>> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cerr << "Failed to parse sound overrides as array\n";
                return false;
            }

            auto* soundOverrides = m_memory.Alloc<SoundOverrideEntry>(valueArray.size());

            auto i = 0u;
            for (const auto& overrideValues : valueArray)
            {
                auto& soundOverride = soundOverrides[i++];

                if (!ParseSingleWeaponAttachment(overrideValues[0], soundOverride.attachment1))
                    return false;

                if (!ParseSingleWeaponAttachment(overrideValues[1], soundOverride.attachment2))
                    return false;

                if (!ParseSoundType(overrideValues[2], soundOverride.soundType))
                    return false;

                ParseSoundAlias(overrideValues[3], soundOverride.overrideSound);
                ParseSoundAlias(overrideValues[4], soundOverride.altmodeSound);
            }

            m_weapon.weapCompleteDef.soundOverrides = soundOverrides;
            m_weapon.weapCompleteDef.numSoundOverrides = valueArray.size();

            return true;
        }

        bool ConvertFxOverrides(const std::string& value)
        {
            std::vector<std::array<std::string, 5>> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cerr << "Failed to parse attachments as array\n";
                return false;
            }

            auto* fxOverrides = m_memory.Alloc<FxOverrideEntry>(valueArray.size());

            auto i = 0u;
            for (const auto& overrideValues : valueArray)
            {
                auto& fxOverride = fxOverrides[i++];

                if (!ParseSingleWeaponAttachment(overrideValues[0], fxOverride.attachment1))
                    return false;

                if (!ParseSingleWeaponAttachment(overrideValues[1], fxOverride.attachment2))
                    return false;

                if (!ParseFxType(overrideValues[2], fxOverride.fxType))
                    return false;

                if (!ParseFxEffectDef(overrideValues[3], fxOverride.overrideFx))
                    return false;

                if (!ParseFxEffectDef(overrideValues[4], fxOverride.altmodeFx))
                    return false;
            }

            m_weapon.weapCompleteDef.fxOverrides = fxOverrides;
            m_weapon.weapCompleteDef.numFxOverrides = valueArray.size();

            return true;
        }

        bool ConvertReloadOverrides(const std::string& value)
        {
            std::vector<std::array<std::string, 3>> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cerr << "Failed to parse reload overrides as array\n";
                return false;
            }

            auto* reloadOverrides = m_memory.Alloc<ReloadStateTimerEntry>(valueArray.size());

            auto i = 0u;
            for (const auto& overrideValues : valueArray)
            {
                auto& reloadOverride = reloadOverrides[i++];

                if (!ParseSingleWeaponAttachment(overrideValues[0], reloadOverride.attachment))
                    return false;

                reloadOverride.unused = 0u;

                if (!ParseInt(overrideValues[1], reloadOverride.reloadAddTime))
                    return false;

                if (!ParseInt(overrideValues[2], reloadOverride.reloadStartAddTime))
                    return false;
            }

            m_weapon.weapCompleteDef.reloadOverrides = reloadOverrides;
            m_weapon.weapCompleteDef.numReloadStateTimerOverrides = valueArray.size();

            return true;
        }

        bool ConvertNoteTrackOverrides(const std::string& value)
        {
            std::vector<std::array<std::string, 3>> valueArray;
            if (!ParseAsArray(value, valueArray))
            {
                std::cerr << "Failed to parse note track overrides as array\n";
                return false;
            }

            std::vector<NoteTrackToSoundEntry> overrideVector;
            NoteTrackToSoundEntry currentOverride;
            auto currentOverrideKeyOffset = 0u;
            currentOverride.unused = 0u;
            std::string lastAttachment;

            auto i = 0u;
            for (const auto& overrideValues : valueArray)
            {
                if (i == 0u || lastAttachment != overrideValues[0])
                {
                    if (currentOverrideKeyOffset > 0u)
                        overrideVector.emplace_back(currentOverride);

                    if (!ParseSingleWeaponAttachment(overrideValues[0], currentOverride.attachment))
                        return false;

                    currentOverride.notetrackSoundMapKeys = m_memory.Alloc<ScriptString>(24u);
                    currentOverride.notetrackSoundMapValues = m_memory.Alloc<ScriptString>(24u);
                    lastAttachment = overrideValues[0];
                    currentOverrideKeyOffset = 0u;
                }

                if (currentOverrideKeyOffset >= 24u)
                {
                    std::cerr << std::format("Cannot have more than {} note track overrides per attachment\n", 24u);
                    return false;
                }

                ParseScriptString(overrideValues[1], currentOverride.notetrackSoundMapKeys[currentOverrideKeyOffset]);
                ParseScriptString(overrideValues[2], currentOverride.notetrackSoundMapValues[currentOverrideKeyOffset]);
                currentOverrideKeyOffset++;
            }

            if (currentOverrideKeyOffset > 0u)
                overrideVector.emplace_back(currentOverride);

            m_weapon.weapCompleteDef.notetrackOverrides = m_memory.Alloc<NoteTrackToSoundEntry>(overrideVector.size());
            memcpy(m_weapon.weapCompleteDef.notetrackOverrides, overrideVector.data(), sizeof(NoteTrackToSoundEntry) * overrideVector.size());
            m_weapon.weapCompleteDef.numNotetrackOverrides = overrideVector.size();

            return true;
        }

        bool ParseSingleWeaponAttachment(const std::string& value, WeaponAttachmentCombination& attachment)
        {
            attachment.fields = 0u;
            if (value == "none")
                return true;

            for (auto i = 0u; i < std::extent_v<decltype(WeaponFullDef::scopes)>; i++)
            {
                const auto* scope = m_weapon.scopes[i];
                if (scope && scope->szInternalName && value == scope->szInternalName)
                {
                    attachment.scope = static_cast<unsigned short>(i + 1);
                    return true;
                }
            }

            for (auto i = 0u; i < std::extent_v<decltype(WeaponFullDef::underBarrels)>; i++)
            {
                const auto* underBarrel = m_weapon.underBarrels[i];
                if (underBarrel && underBarrel->szInternalName && value == underBarrel->szInternalName)
                {
                    attachment.underBarrel = static_cast<unsigned short>(i + 1);
                    return true;
                }
            }

            for (auto i = 0u; i < std::extent_v<decltype(WeaponFullDef::others)>; i++)
            {
                const auto* other = m_weapon.others[i];
                if (other && other->szInternalName && value == other->szInternalName)
                {
                    attachment.other = static_cast<unsigned short>(1u << i);
                    return true;
                }
            }

            std::cerr << std::format("Weapon does not have attachment \"{}\"\n", value);
            return false;
        }

        void ParseAnim(const std::string& value, const char*& animName)
        {
            if (value == "none")
            {
                animName = nullptr;
                return;
            }

            animName = m_memory.Dup(value.c_str());
            m_registration.AddIndirectAssetReference(m_context.LoadIndirectAssetReference<AssetXAnim>(value));
        }

        void ParseSoundAlias(const std::string& value, SndAliasCustom& soundAlias)
        {
            if (value == "none")
            {
                soundAlias.name = nullptr;
                return;
            }

            soundAlias.name = m_memory.Alloc<snd_alias_list_name>();
            soundAlias.name->soundName = m_memory.Dup(value.c_str());
            m_registration.AddIndirectAssetReference(m_context.LoadIndirectAssetReference<AssetSound>(value));
        }

        bool ParseFxEffectDef(const std::string& value, FxEffectDef*& fx)
        {
            if (value == "none")
            {
                fx = nullptr;
                return true;
            }

            auto* fxInfo = m_context.LoadDependency<AssetFx>(value);
            if (!fxInfo)
            {
                std::cerr << std::format("Failed to load fx for override \"{}\"\n", value);
                return false;
            }

            fx = fxInfo->Asset();
            m_registration.AddDependency(fxInfo);

            return true;
        }

        static bool ParseAnimFile(const std::string& value, weapAnimFiles_t& animFile)
        {
            for (auto i = 0u; i < std::extent_v<decltype(weapAnimFilesNames)>; i++)
            {
                if (value == weapAnimFilesNames[i])
                {
                    animFile = static_cast<weapAnimFiles_t>(i);
                    return true;
                }
            }

            std::cerr << std::format("Unknown anim file \"{}\"\n", value);
            return false;
        }

        static bool ParseSoundType(const std::string& value, SoundOverrideTypes& soundType)
        {
            for (auto i = 0u; i < std::extent_v<decltype(soundOverrideTypeNames)>; i++)
            {
                if (value == soundOverrideTypeNames[i])
                {
                    soundType = static_cast<SoundOverrideTypes>(i);
                    return true;
                }
            }

            std::cerr << std::format("Unknown sound type \"{}\"\n", value);
            return false;
        }

        static bool ParseFxType(const std::string& value, FxOverrideTypes& fxType)
        {
            for (auto i = 0u; i < std::extent_v<decltype(fxOverrideTypeNames)>; i++)
            {
                if (value == fxOverrideTypeNames[i])
                {
                    fxType = static_cast<FxOverrideTypes>(i);
                    return true;
                }
            }

            std::cerr << std::format("Unknown fx type \"{}\"\n", value);
            return false;
        }

        static bool ParseInt(const std::string& value, int& out)
        {
            size_t size;
            out = std::stoi(value, &size);

            if (size != value.size())
            {
                std::cerr << std::format("Invalid int value: \"{}\"\n", value);
                return false;
            }

            return true;
        }

        void ParseScriptString(const std::string& value, ScriptString& out)
        {
            out = m_zone_script_strings.AddOrGetScriptString(value);
            m_registration.AddScriptString(out);
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

            case WFT_PER_SURFACE_TYPE_SOUND:
                return ConvertPerSurfaceTypeSound(field, value);

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
                return ConvertNoteTrackMap(field, value, "sound", std::extent_v<decltype(WeaponFullDef::notetrackSoundMapKeys)>);

            case WFT_NOTETRACKRUMBLEMAP:
                return ConvertNoteTrackMap(field, value, "rumble", std::extent_v<decltype(WeaponFullDef::notetrackRumbleMapKeys)>);

            case WFT_ANIM_NAME:
                return ConvertAnimName(field, value);

            case WFT_ATTACHMENT:
                return ConvertAttachments(value);

            case WFT_ANIM_OVERRIDES:
                return ConvertAnimOverrides(value);

            case WFT_SOUND_OVERRIDES:
                return ConvertSoundOverrides(value);

            case WFT_FX_OVERRIDES:
                return ConvertFxOverrides(value);

            case WFT_RELOAD_OVERRIDES:
                return ConvertReloadOverrides(value);

            case WFT_NOTETRACK_OVERRIDES:
                return ConvertNoteTrackOverrides(value);

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
            : InfoStringToStructConverter(infoString, &weaponFullDef, zoneScriptStrings, memory, context, registration, fields, fieldCount),
              m_weapon(weaponFullDef)
        {
        }

    private:
        WeaponFullDef& m_weapon;
    };

    void InitWeaponFullDef(WeaponFullDef& weapon)
    {
        weapon.weapCompleteDef.weapDef = &weapon.weapDef;
        weapon.weapCompleteDef.hideTags = weapon.hideTags;
        weapon.weapCompleteDef.szXAnims = weapon.szXAnims;
        weapon.weapCompleteDef.scopes = weapon.scopes;
        weapon.weapCompleteDef.underBarrels = weapon.underBarrels;
        weapon.weapCompleteDef.others = weapon.others;
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
            std::cerr << std::format("Projectile speed for WeapType {} must be greater than 0.0", weaponCompleteDef.szDisplayName);
        if (weaponDef.destabilizationCurvatureMax >= 1000000000.0f || weaponDef.destabilizationCurvatureMax < 0.0f)
            std::cerr << std::format("Destabilization angle for for WeapType {} must be between 0 and 45 degrees", weaponCompleteDef.szDisplayName);
        if (weaponDef.destabilizationRateTime < 0.0f)
            std::cerr << std::format("Destabilization rate time for for WeapType {} must be non-negative", weaponCompleteDef.szDisplayName);
    }

    void CheckTurretBarrelSpin(const WeaponCompleteDef& weaponCompleteDef, const WeaponDef& weaponDef)
    {
        if (weaponDef.weapClass != WEAPCLASS_TURRET)
            std::cerr << std::format("Rotating barrel set for non-turret weapon {}.", weaponCompleteDef.szInternalName);

        if (0.0f == weaponDef.turretBarrelSpinSpeed)
        {
            std::cerr << std::format(
                "Rotating barrel spin speed '{}' is invalid for weapon {}.", weaponDef.turretBarrelSpinSpeed, weaponCompleteDef.szInternalName);
        }
        if (0.0f < weaponDef.turretOverheatUpRate && 0.0f >= weaponDef.turretOverheatDownRate)
        {
            std::cerr << std::format("Turret overheat Up rate is set, but the down rate '{}' is invalid for weapon {}.",
                                     weaponDef.turretOverheatDownRate,
                                     weaponCompleteDef.szInternalName);
        }
    }

    void CheckThermalScope(const WeaponCompleteDef& weaponCompleteDef, const WeaponDef& weaponDef)
    {
        if (0.0f != weaponDef.fAdsZoomInFrac)
        {
            std::cerr << std::format("Weapon {} ({}) has thermal scope set. ADS Zoom In frac should be 0 to prevent zoom-in blur ({}).\n",
                                     weaponCompleteDef.szInternalName,
                                     weaponCompleteDef.szDisplayName,
                                     weaponDef.fAdsZoomInFrac);
        }

        if (0.0f != weaponDef.fAdsZoomOutFrac)
        {
            std::cerr << std::format("Weapon {} ({}) has thermal scope set. ADS Zoom Out frac should be 0 to prevent zoom-out blur ({}).\n",
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
            std::cerr << std::format("Enemy crosshair ranges should be less than {}\n", 15000.0f);

        if (weaponDef.weapType == WEAPTYPE_PROJECTILE)
            CheckProjectileValues(weaponCompleteDef, weaponDef);

        if (weaponDef.turretBarrelSpinEnabled)
            CheckTurretBarrelSpin(weaponCompleteDef, weaponDef);

        if (weaponDef.thermalScope)
            CheckThermalScope(weaponCompleteDef, weaponDef);

        if (weaponDef.offhandClass && !weaponDef.bClipOnly)
        {
            std::cerr << std::format(
                "Weapon {} ({}) is an offhand weapon but is not set to clip only, which is not supported since we can't reload the offhand slot.\n",
                weaponCompleteDef.szInternalName,
                weaponCompleteDef.szDisplayName);
        }

        if (weaponDef.weapType == WEAPTYPE_BULLET)
        {
            if (weaponDef.bulletExplDmgMult <= 0.0f)
                std::cerr << std::format("Detected invalid bulletExplDmgMult of '{}' for weapon '{}'; please update weapon settings.\n",
                                         weaponDef.bulletExplDmgMult,
                                         weaponCompleteDef.szInternalName);
            if (weaponDef.bulletExplRadiusMult <= 0.0f)
                std::cerr << std::format("Detected invalid bulletExplRadiusMult of '{}' for weapon '{}'; please update weapon settings.\n",
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
        auto* accuracyGraphLoader = context.GetZoneAssetLoaderState<AccuracyGraphLoader>();

        if (weaponFullDef.weapDef.aiVsAiAccuracyGraphName && weaponFullDef.weapDef.aiVsAiAccuracyGraphName[0])
        {
            const auto* graph = accuracyGraphLoader->LoadAiVsAiGraph(searchPath, weaponFullDef.weapDef.aiVsAiAccuracyGraphName);
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
            const auto* graph = accuracyGraphLoader->LoadAiVsPlayerGraph(searchPath, weaponFullDef.weapDef.aiVsPlayerAccuracyGraphName);
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

InfoStringLoaderWeapon::InfoStringLoaderWeapon(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    : m_memory(memory),
      m_search_path(searchPath),
      m_zone(zone)
{
}

AssetCreationResult InfoStringLoaderWeapon::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
{
    auto* weaponFullDef = m_memory.Alloc<WeaponFullDef>();

    InitWeaponFullDef(*weaponFullDef);
    weaponFullDef->weapCompleteDef.szInternalName = m_memory.Dup(assetName.c_str());

    AssetRegistration<AssetWeapon> registration(assetName, &weaponFullDef->weapCompleteDef);

    InfoStringToWeaponConverter converter(
        infoString, *weaponFullDef, m_zone.m_script_strings, m_memory, context, registration, weapon_fields, std::extent_v<decltype(weapon_fields)>);
    if (!converter.Convert())
    {
        std::cerr << std::format("Failed to parse weapon: \"{}\"\n", assetName);
        return AssetCreationResult::Failure();
    }

    CalculateWeaponFields(*weaponFullDef, m_memory);

    LoadAccuracyGraphs(*weaponFullDef, m_memory, m_search_path, context);

    return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
}
