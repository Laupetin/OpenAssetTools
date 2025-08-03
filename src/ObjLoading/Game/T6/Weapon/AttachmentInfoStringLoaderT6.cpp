#include "AttachmentInfoStringLoaderT6.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/T6.h"
#include "Game/T6/Weapon/AttachmentFields.h"
#include "Game/T6/Weapon/WeaponStrings.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace T6;

namespace
{
    eAttachmentPoint attachmentPointByAttachmentTable[]{
        ATTACHMENT_POINT_NONE,    // none
        ATTACHMENT_POINT_TOP,     // acog
        ATTACHMENT_POINT_TRIGGER, // dualclip
        ATTACHMENT_POINT_TOP,     // dualoptic
        ATTACHMENT_POINT_BOTTOM,  // dw
        ATTACHMENT_POINT_MUZZLE,  // extbarrel
        ATTACHMENT_POINT_TRIGGER, // extclip
        ATTACHMENT_POINT_TRIGGER, // extramags
        ATTACHMENT_POINT_GUNPERK, // fastads
        ATTACHMENT_POINT_TOP,     // fastreload
        ATTACHMENT_POINT_TRIGGER, // fmj
        ATTACHMENT_POINT_BOTTOM,  // gl
        ATTACHMENT_POINT_BOTTOM,  // grip
        ATTACHMENT_POINT_TOP,     // holo
        ATTACHMENT_POINT_BOTTOM,  // ir
        ATTACHMENT_POINT_BOTTOM,  // is
        ATTACHMENT_POINT_GUNPERK, // longbreath
        ATTACHMENT_POINT_BOTTOM,  // mk
        ATTACHMENT_POINT_TOP,     // mms
        ATTACHMENT_POINT_TOP,     // rangefinder
        ATTACHMENT_POINT_TOP,     // reflex
        ATTACHMENT_POINT_MUZZLE,  // rf
        ATTACHMENT_POINT_BOTTOM,  // sf
        ATTACHMENT_POINT_MUZZLE,  // silencer
        ATTACHMENT_POINT_TRIGGER, // stackfire
        ATTACHMENT_POINT_GUNPERK, // stalker
        ATTACHMENT_POINT_GUNPERK, // steadyaim
        ATTACHMENT_POINT_GUNPERK, // swayreduc
        ATTACHMENT_POINT_TOP,     // tacknife
        ATTACHMENT_POINT_TOP,     // vzoom
    };
    static_assert(std::extent_v<decltype(attachmentPointByAttachmentTable)> == ATTACHMENT_TYPE_COUNT);

    class InfoStringToAttachmentConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<attachmentFieldType_t>(field.iFieldType))
            {
            case AFT_ATTACHMENTTYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szAttachmentTypeNames, std::extent_v<decltype(szAttachmentTypeNames)>);

            case AFT_PENETRATE_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, penetrateTypeNames, std::extent_v<decltype(penetrateTypeNames)>);

            case AFT_FIRETYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, szWeapFireTypeNames, std::extent_v<decltype(szWeapFireTypeNames)>);

            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToAttachmentConverter(const InfoString& infoString,
                                        WeaponAttachment& attachment,
                                        ZoneScriptStrings& zoneScriptStrings,
                                        MemoryManager& memory,
                                        AssetCreationContext& context,
                                        AssetRegistration<AssetAttachment>& registration,
                                        const cspField_t* fields,
                                        const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &attachment, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };

    void CalculateAttachmentFields(WeaponAttachment& attachment)
    {
        assert(static_cast<unsigned>(attachment.attachmentType) < ATTACHMENT_TYPE_COUNT);
        if (static_cast<unsigned>(attachment.attachmentType) < ATTACHMENT_TYPE_COUNT)
            attachment.attachmentPoint = attachmentPointByAttachmentTable[attachment.attachmentType];
    }
} // namespace

namespace T6::attachment
{
    InfoStringLoader::InfoStringLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
        : m_memory(memory),
          m_search_path(searchPath),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoader::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        auto* attachment = m_memory.Alloc<WeaponAttachment>();
        attachment->szInternalName = m_memory.Dup(assetName.c_str());

        AssetRegistration<AssetAttachment> registration(assetName, attachment);

        InfoStringToAttachmentConverter converter(
            infoString, *attachment, m_zone.m_script_strings, m_memory, context, registration, attachment_fields, std::extent_v<decltype(attachment_fields)>);
        if (!converter.Convert())
        {
            std::cerr << std::format("Failed to parse attachment: \"{}\"\n", assetName);
            return AssetCreationResult::Failure();
        }

        CalculateAttachmentFields(*attachment);

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace T6::attachment
