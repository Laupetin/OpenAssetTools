#include "AssetLoaderWeaponAttachmentUnique.h"

#include <cstring>
#include <iostream>
#include <cassert>

#include "Utils/ClassUtils.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/InfoString/WeaponAttachmentUniqueFields.h"
#include "InfoString/InfoString.h"

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
                std::cout << "Failed to parse hide tags as array" << std::endl;
                return false;
            }

            if (valueArray.size() > std::extent<decltype(WeaponFullDef::hideTags)>::value)
            {
                std::cout << "Cannot have more than " << std::extent<decltype(WeaponFullDef::hideTags)>::value << " hide tags!" << std::endl;
                return false;
            }

            auto* hideTags = reinterpret_cast<scr_string_t*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
            auto currentHideTag = 0u;

            if (valueArray.size() < std::extent<decltype(WeaponFullDef::hideTags)>::value)
            {
                m_used_script_string_list.emplace(m_zone_script_strings.AddOrGetScriptString(""));
            }

            for (; currentHideTag < valueArray.size(); currentHideTag++)
            {
                const auto scrString = m_zone_script_strings.AddOrGetScriptString(valueArray[currentHideTag]);
                hideTags[currentHideTag] = scrString;
                m_used_script_string_list.emplace(scrString);
            }

            for (; currentHideTag < std::extent<decltype(WeaponFullDef::hideTags)>::value; currentHideTag++)
            {
                hideTags[currentHideTag] = m_zone_script_strings.GetScriptString("");
            }

            return true;
        }

        _NODISCARD bool ConvertWeaponCamo(const cspField_t& field, const std::string& value)
        {
            if (value.empty())
            {
                *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* camo = m_loading_manager->LoadDependency(ASSET_TYPE_WEAPON_CAMO, value);

            if (camo == nullptr)
            {
                std::cout << "Failed to load camo asset \"" << value << "\"" << std::endl;
                return false;
            }

            m_dependencies.emplace(camo);
            *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = camo->m_ptr;

            return true;
        }

    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<attachmentUniqueFieldType_t>(field.iFieldType))
            {
            case AUFT_ATTACHMENTTYPE:
                return ConvertEnumInt(value, field.iOffset, szAttachmentTypeNames, std::extent<decltype(szAttachmentTypeNames)>::value);

            case AUFT_HIDETAGS:
                return ConvertHideTags(field, value);

            case AUFT_OVERLAYRETICLE:
                return ConvertEnumInt(value, field.iOffset, szWeapOverlayReticleNames, std::extent<decltype(szWeapOverlayReticleNames)>::value);

            case AUFT_CAMO:
                return ConvertWeaponCamo(field, value);

            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToWeaponAttachmentUniqueConverter(const InfoString& infoString, WeaponAttachmentUniqueFull* attachmentUniqueFull, ZoneScriptStrings& zoneScriptStrings, MemoryManager* memory, IAssetLoadingManager* manager,
            const cspField_t* fields, const size_t fieldCount)
            : InfoStringToStructConverter(infoString, attachmentUniqueFull, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
}

bool AssetLoaderWeaponAttachmentUnique::ExtractAttachmentsFromAssetName(const std::string& assetName, std::vector<eAttachment>& attachmentList)
{
    std::vector<std::string> parts;

    auto attachCount = 1u;
    auto partStart = 0u;
    for(auto ci = 0u; ci < assetName.size(); ci++)
    {
        if(assetName[ci] == '_')
        {
            parts.emplace_back(assetName, partStart, ci - partStart);
            partStart = ci + 1;
        }
        else if(assetName[ci] == '+')
        {
            attachCount++;
            parts.emplace_back(assetName, partStart, ci - partStart);
            partStart = ci + 1;
        }
    }

    if(partStart < assetName.size())
        parts.emplace_back(assetName, partStart, assetName.size() - partStart);

    for(auto attachPartOffset = parts.size() - attachCount; attachPartOffset < parts.size(); attachPartOffset++)
    {
        auto& specifiedAttachName = parts[attachPartOffset];

        for (auto& c : specifiedAttachName)
            c = static_cast<char>(tolower(c));

        auto foundAttachment = false;
        for(auto attachIndex = 0u; attachIndex < std::extent<decltype(szAttachmentTypeNames)>::value; attachIndex++)
        {
            if(specifiedAttachName == szAttachmentTypeNames[attachIndex])
            {
                attachmentList.push_back(static_cast<eAttachment>(attachIndex));
                foundAttachment = true;
                break;
            }
        }

        if(!foundAttachment)
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
    if(!ExtractAttachmentsFromAssetName(assetName, attachmentsFromName))
    {
        std::cout << "Failed to determine attachments from attachment unique name \"" << assetName << "\"" << std::endl;
        return false;
    }

    if (attachmentsFromName.size() > 1)
    {
        for(auto attachment : attachmentsFromName)
        {
            attachmentUnique->attachment.combinedAttachmentTypeMask |= 1 << attachment;
        }
    }

    // animationOverrides
    // siblingLink
    // childLink
    // soundOverrides
    // effectOverrides

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

bool AssetLoaderWeaponAttachmentUnique::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderWeaponAttachmentUnique::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "attachmentunique/" + assetName;
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT_UNIQUE, *file.m_stream))
    {
        std::cout << "Failed to read attachment unique raw file: \"" << fileName << "\"" << std::endl;
        return true;
    }

    auto* attachmentUniqueFull = memory->Create<WeaponAttachmentUniqueFull>();
    memset(attachmentUniqueFull, 0, sizeof(WeaponAttachmentUniqueFull));
    LinkAttachmentUniqueFullSubStructs(attachmentUniqueFull);

    InfoStringToWeaponAttachmentUniqueConverter converter(infoString, attachmentUniqueFull, zone->m_script_strings, memory, manager, attachment_unique_fields, std::extent<decltype(attachment_unique_fields)>::value);
    if (!converter.Convert())
    {
        std::cout << "Failed to parse attachment unique raw file: \"" << fileName << "\"" << std::endl;
        return true;
    }

    if (!CalculateAttachmentUniqueFields(assetName, attachmentUniqueFull))
        return true;

    attachmentUniqueFull->attachment.szInternalName = memory->Dup(assetName.c_str());

    auto* assetInfo = GlobalAssetPool<WeaponAttachmentUnique>::GetAssetByName(assetName);
    auto* asset = assetInfo ? assetInfo->Asset() : nullptr;

    manager->AddAsset(ASSET_TYPE_ATTACHMENT_UNIQUE, assetName, &attachmentUniqueFull->attachment, converter.GetDependencies(), converter.GetUsedScriptStrings());

    return true;
}
