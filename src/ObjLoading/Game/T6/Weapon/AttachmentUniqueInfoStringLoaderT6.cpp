#include "AttachmentUniqueInfoStringLoaderT6.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/T6.h"
#include "Game/T6/Weapon/AttachmentUniqueFields.h"
#include "Game/T6/Weapon/WeaponStrings.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace T6;

namespace
{
    class InfoStringToAttachmentUniqueConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<attachmentUniqueFieldType_t>(field.iFieldType))
            {
            case AUFT_ATTACHMENTTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szAttachmentTypeNames, std::extent_v<decltype(szAttachmentTypeNames)>);

            case AUFT_HIDETAGS:
                return ConvertHideTags(field, value);

            case AUFT_OVERLAYRETICLE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapOverlayReticleNames, std::extent_v<decltype(szWeapOverlayReticleNames)>);

            case AUFT_CAMO:
                return ConvertWeaponCamo(field, value);

            case AUFT_ANIM_NAME:
                return ConvertAnimName(field, value);

            default:
                assert(false);
                return false;
            }
        }

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
                m_registration.AddScriptString(m_zone_script_strings.AddOrGetScriptString(nullptr));

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

        [[nodiscard]] bool ConvertWeaponCamo(const cspField_t& field, const std::string& value)
        {
            if (value.empty())
            {
                *reinterpret_cast<WeaponCamo**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = nullptr;
                return true;
            }

            auto* camo = m_context.LoadDependency<AssetWeaponCamo>(value);

            if (camo == nullptr)
            {
                std::cerr << std::format("Failed to load camo asset \"{}\"\n", value);
                return false;
            }

            m_registration.AddDependency(camo);
            *reinterpret_cast<WeaponCamo**>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset) = camo->Asset();

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

    public:
        InfoStringToAttachmentUniqueConverter(const InfoString& infoString,
                                              WeaponAttachmentUniqueFull& attachmentUniqueFull,
                                              ZoneScriptStrings& zoneScriptStrings,
                                              MemoryManager& memory,
                                              AssetCreationContext& context,
                                              AssetRegistration<AssetAttachmentUnique>& registration,
                                              const cspField_t* fields,
                                              const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &attachmentUniqueFull, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };

    void LinkAttachmentUniqueFullSubStructs(WeaponAttachmentUniqueFull& attachmentUnique)
    {
        attachmentUnique.attachment.hideTags = attachmentUnique.hideTags;
        attachmentUnique.attachment.szXAnims = attachmentUnique.szXAnims;
        attachmentUnique.attachment.locationDamageMultipliers = attachmentUnique.locationDamageMultipliers;
    }

    bool CalculateAttachmentUniqueFields(const std::string& assetName, WeaponAttachmentUniqueFull& attachmentUnique)
    {
        // combinedAttachmentTypeMask
        std::vector<eAttachment> attachmentsFromName;
        if (!T6::attachment_unique::ExtractAttachmentsFromAssetName(assetName, attachmentsFromName))
        {
            std::cerr << std::format("Failed to determine attachments from attachment unique name \"{}\"\n", assetName);
            return false;
        }

        if (attachmentsFromName.size() > 1)
        {
            for (const auto attachment : attachmentsFromName)
            {
                attachmentUnique.attachment.combinedAttachmentTypeMask |= 1 << attachment;
            }
        }

        return true;
    }
} // namespace

namespace T6::attachment_unique
{
    bool ExtractAttachmentsFromAssetName(const std::string& assetName, std::vector<eAttachment>& attachmentList)
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

    InfoStringLoader::InfoStringLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
        : m_memory(memory),
          m_search_path(searchPath),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoader::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        auto* attachmentUniqueFull = m_memory.Alloc<WeaponAttachmentUniqueFull>();
        attachmentUniqueFull->attachment.szInternalName = m_memory.Dup(assetName.c_str());

        LinkAttachmentUniqueFullSubStructs(*attachmentUniqueFull);

        AssetRegistration<AssetAttachmentUnique> registration(assetName, &attachmentUniqueFull->attachment);

        InfoStringToAttachmentUniqueConverter converter(infoString,
                                                        *attachmentUniqueFull,
                                                        m_zone.m_script_strings,
                                                        m_memory,
                                                        context,
                                                        registration,
                                                        attachment_unique_fields,
                                                        std::extent_v<decltype(attachment_unique_fields)>);
        if (!converter.Convert())
        {
            std::cerr << std::format("Failed to parse attachment unique: \"{}\"\n", assetName);
            return AssetCreationResult::Failure();
        }

        CalculateAttachmentUniqueFields(assetName, *attachmentUniqueFull);

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace T6::attachment_unique
