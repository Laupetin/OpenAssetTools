#include "AssetLoaderWeaponAttachmentUnique.h"

#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/InfoString/WeaponAttachmentUniqueFields.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "Utils/ClassUtils.h"

#include <cassert>
#include <cstring>
#include <iostream>

using namespace T6;

namespace T6
{
    class InfoStringToWeaponAttachmentUniqueConverter final : public InfoStringToStructConverter
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
                std::cerr << "Cannot have more than " << std::extent_v<decltype(WeaponFullDef::hideTags)> << " hide tags!\n";
                return false;
            }

            auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);

            if (valueArray.size() < std::extent_v<decltype(WeaponFullDef::hideTags)>)
            {
                m_used_script_string_list.emplace(m_zone_script_strings.AddOrGetScriptString(nullptr));
            }

            auto currentHideTag = 0u;
            for (; currentHideTag < valueArray.size(); currentHideTag++)
            {
                const auto& currentValue = valueArray[currentHideTag];
                const auto scrString =
                    !currentValue.empty() ? m_zone_script_strings.AddOrGetScriptString(currentValue) : m_zone_script_strings.AddOrGetScriptString(nullptr);
                hideTags[currentHideTag] = scrString;
                m_used_script_string_list.emplace(scrString);
            }

            for (; currentHideTag < std::extent_v<decltype(WeaponFullDef::hideTags)>; currentHideTag++)
            {
                hideTags[currentHideTag] = m_zone_script_strings.GetScriptString(nullptr);
            }

            return true;
        }

        _NODISCARD bool ConvertWeaponCamo(const cspField_t& field, const std::string& value)
        {
            if (value.empty())
            {
                *reinterpret_cast<WeaponCamo**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* camo = m_loading_manager->LoadDependency<AssetWeaponCamo>(value);

            if (camo == nullptr)
            {
                std::cerr << "Failed to load camo asset \"" << value << "\"\n";
                return false;
            }

            m_dependencies.emplace(camo);
            *reinterpret_cast<WeaponCamo**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = camo->Asset();

            return true;
        }

        bool ConvertAnimName(const cspField_t& field, const std::string& value)
        {
            if (ConvertString(value, field.iOffset))
            {
                if (!value.empty())
                    m_indirect_asset_references.emplace(m_loading_manager->LoadIndirectAssetReference<AssetXAnim>(value));

                return true;
            }

            return false;
        }

    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<attachmentUniqueFieldType_t>(field.iFieldType))
            {
            case AUFT_ATTACHMENTTYPE:
                return ConvertEnumInt(value, field.iOffset, szAttachmentTypeNames, std::extent_v<decltype(szAttachmentTypeNames)>);

            case AUFT_HIDETAGS:
                return ConvertHideTags(field, value);

            case AUFT_OVERLAYRETICLE:
                return ConvertEnumInt(value, field.iOffset, szWeapOverlayReticleNames, std::extent_v<decltype(szWeapOverlayReticleNames)>);

            case AUFT_CAMO:
                return ConvertWeaponCamo(field, value);

            case AUFT_ANIM_NAME:
                return ConvertAnimName(field, value);

            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToWeaponAttachmentUniqueConverter(const InfoString& infoString,
                                                    WeaponAttachmentUniqueFull* attachmentUniqueFull,
                                                    ZoneScriptStrings& zoneScriptStrings,
                                                    MemoryManager* memory,
                                                    IAssetLoadingManager* manager,
                                                    const cspField_t* fields,
                                                    const size_t fieldCount)
            : InfoStringToStructConverter(infoString, attachmentUniqueFull, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
} // namespace T6

bool AssetLoaderWeaponAttachmentUnique::ExtractAttachmentsFromAssetName(const std::string& assetName, std::vector<eAttachment>& attachmentList)
{
    std::vector<std::string> parts;

    auto attachCount = 1u;
    auto partStart = 0u;
    for (auto ci = 0u; ci < assetName.size(); ci++)
    {
        if (assetName[ci] == '_')
        {
            parts.emplace_back(assetName, partStart, ci - partStart);
            partStart = ci + 1;
        }
        else if (assetName[ci] == '+')
        {
            attachCount++;
            parts.emplace_back(assetName, partStart, ci - partStart);
            partStart = ci + 1;
        }
    }

    if (partStart < assetName.size())
        parts.emplace_back(assetName, partStart, assetName.size() - partStart);

    for (auto attachPartOffset = parts.size() - attachCount; attachPartOffset < parts.size(); attachPartOffset++)
    {
        auto& specifiedAttachName = parts[attachPartOffset];

        for (auto& c : specifiedAttachName)
            c = static_cast<char>(tolower(c));

        auto foundAttachment = false;
        for (auto attachIndex = 0u; attachIndex < std::extent_v<decltype(szAttachmentTypeNames)>; attachIndex++)
        {
            if (specifiedAttachName == szAttachmentTypeNames[attachIndex])
            {
                attachmentList.push_back(static_cast<eAttachment>(attachIndex));
                foundAttachment = true;
                break;
            }
        }

        if (!foundAttachment)
            return false;
    }

    return true;
}

void AssetLoaderWeaponAttachmentUnique::LinkAttachmentUniqueFullSubStructs(WeaponAttachmentUniqueFull* attachmentUnique)
{
    attachmentUnique->attachment.hideTags = attachmentUnique->hideTags;
    attachmentUnique->attachment.szXAnims = attachmentUnique->szXAnims;
    attachmentUnique->attachment.locationDamageMultipliers = attachmentUnique->locationDamageMultipliers;
}

bool AssetLoaderWeaponAttachmentUnique::CalculateAttachmentUniqueFields(const std::string& assetName, WeaponAttachmentUniqueFull* attachmentUnique)
{
    // combinedAttachmentTypeMask
    std::vector<eAttachment> attachmentsFromName;
    if (!ExtractAttachmentsFromAssetName(assetName, attachmentsFromName))
    {
        std::cerr << "Failed to determine attachments from attachment unique name \"" << assetName << "\"\n";
        return false;
    }

    if (attachmentsFromName.size() > 1)
    {
        for (const auto attachment : attachmentsFromName)
        {
            attachmentUnique->attachment.combinedAttachmentTypeMask |= 1 << attachment;
        }
    }

    return true;
}

bool AssetLoaderWeaponAttachmentUnique::LoadFromInfoString(
    const InfoString& infoString, const std::string& assetName, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone)
{

    auto* attachmentUniqueFull = memory->Create<WeaponAttachmentUniqueFull>();
    memset(attachmentUniqueFull, 0, sizeof(WeaponAttachmentUniqueFull));
    LinkAttachmentUniqueFullSubStructs(attachmentUniqueFull);

    InfoStringToWeaponAttachmentUniqueConverter converter(
        infoString, attachmentUniqueFull, zone->m_script_strings, memory, manager, attachment_unique_fields, std::extent_v<decltype(attachment_unique_fields)>);
    if (!converter.Convert())
    {
        std::cerr << "Failed to parse attachment unique: \"" << assetName << "\"\n";
        return true;
    }

    if (!CalculateAttachmentUniqueFields(assetName, attachmentUniqueFull))
        return true;

    attachmentUniqueFull->attachment.szInternalName = memory->Dup(assetName.c_str());

    manager->AddAsset(ASSET_TYPE_ATTACHMENT_UNIQUE,
                      assetName,
                      &attachmentUniqueFull->attachment,
                      converter.GetDependencies(),
                      converter.GetUsedScriptStrings(),
                      converter.GetIndirectAssetReferences());

    return true;
}

void* AssetLoaderWeaponAttachmentUnique::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* attachmentUniqueFull = memory->Create<WeaponAttachmentUniqueFull>();
    memset(attachmentUniqueFull, 0, sizeof(WeaponAttachmentUniqueFull));
    LinkAttachmentUniqueFullSubStructs(attachmentUniqueFull);
    CalculateAttachmentUniqueFields(assetName, attachmentUniqueFull);
    attachmentUniqueFull->attachment.szInternalName = memory->Dup(assetName.c_str());
    return attachmentUniqueFull;
}

bool AssetLoaderWeaponAttachmentUnique::CanLoadFromGdt() const
{
    return true;
}

bool AssetLoaderWeaponAttachmentUnique::LoadFromGdt(
    const std::string& assetName, IGdtQueryable* gdtQueryable, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto* gdtEntry = gdtQueryable->GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_WEAPON_ATTACHMENT_UNIQUE, assetName);
    if (gdtEntry == nullptr)
        return false;

    InfoString infoString;
    if (!infoString.FromGdtProperties(*gdtEntry))
    {
        std::cerr << "Failed to read attachment unique gdt entry: \"" << assetName << "\"\n";
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}

bool AssetLoaderWeaponAttachmentUnique::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderWeaponAttachmentUnique::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "attachmentunique/" + assetName;
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT_UNIQUE, *file.m_stream))
    {
        std::cerr << "Failed to read attachment unique raw file: \"" << fileName << "\"\n";
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}
