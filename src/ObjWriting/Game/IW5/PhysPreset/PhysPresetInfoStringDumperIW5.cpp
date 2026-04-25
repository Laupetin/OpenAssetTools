#include "PhysPresetInfoStringDumperIW5.h"

#include "Game/IW5/InfoString/EnumStrings.h"
#include "Game/IW5/InfoString/InfoStringFromStructConverter.h"
#include "Game/IW5/ObjConstantsIW5.h"
#include "Game/IW5/PhysPreset/PhysPresetFields.h"
#include "PhysPreset/PhysPresetCommon.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

using namespace IW5;

namespace
{
    class InfoStringFromPhysPresetConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<physPresetFieldType_t>(field.iFieldType))
            {
            case PPFT_SCALING:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szPhysPresetScalingNames, std::extent_v<decltype(szPhysPresetScalingNames)>);
                break;

            case PPFT_NUM_FIELD_TYPES:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromPhysPresetConverter(const PhysPresetInfo* structure,
                                          const cspField_t* fields,
                                          const size_t fieldCount,
                                          std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };

    void CopyToPhysPresetInfo(const PhysPreset* physPreset, PhysPresetInfo* physPresetInfo)
    {
        physPresetInfo->name = physPreset->name;
        physPresetInfo->type = physPreset->type;
        physPresetInfo->mass = std::clamp(physPreset->mass, 1.0f, 2000.0f);
        physPresetInfo->bounce = physPreset->bounce;
        physPresetInfo->friction = physPreset->friction;
        physPresetInfo->bulletForceScale = physPreset->bulletForceScale;
        physPresetInfo->explosiveForceScale = physPreset->explosiveForceScale;
        physPresetInfo->sndAliasPrefix = physPreset->sndAliasPrefix;
        physPresetInfo->piecesSpreadFraction = physPreset->piecesSpreadFraction;
        physPresetInfo->piecesUpwardVelocity = physPreset->piecesUpwardVelocity;
        physPresetInfo->minMomentum = physPreset->minMomentum;
        physPresetInfo->maxMomentum = physPreset->maxMomentum;
        physPresetInfo->minPitch = physPreset->minPitch;
        physPresetInfo->maxPitch = physPreset->maxPitch;
        if (physPreset->volumeType == PHYSPRESET_SCALING_LINEAR || physPreset->volumeType == PHYSPRESET_SCALING_QUADRATIC)
        {
            physPresetInfo->volumeType = physPreset->volumeType;
        }
        else
        {
            // If type is invalid, assume linear
            physPresetInfo->volumeType = PHYSPRESET_SCALING_LINEAR;
        }
        if (physPreset->pitchType == PHYSPRESET_SCALING_LINEAR || physPreset->pitchType == PHYSPRESET_SCALING_QUADRATIC)
        {
            physPresetInfo->pitchType = physPreset->pitchType;
        }
        else
        {
            // If type is invalid, assume linear
            physPresetInfo->pitchType = PHYSPRESET_SCALING_LINEAR;
        }
        physPresetInfo->tempDefaultToCylinder = physPreset->tempDefaultToCylinder ? 1 : 0;
        physPresetInfo->perSurfaceSndAlias = physPreset->perSurfaceSndAlias ? 1 : 0;
    }

    InfoString CreateInfoString(const XAssetInfo<PhysPreset>& asset)
    {
        auto* physPresetInfo = new PhysPresetInfo;
        CopyToPhysPresetInfo(asset.Asset(), physPresetInfo);

        InfoStringFromPhysPresetConverter converter(physPresetInfo,
                                                    phys_preset_fields,
                                                    std::extent_v<decltype(phys_preset_fields)>,
                                                    [asset](const scr_string_t scrStr) -> std::string
                                                    {
                                                        assert(scrStr < asset.m_zone->m_script_strings.Count());
                                                        if (scrStr >= asset.m_zone->m_script_strings.Count())
                                                            return "";

                                                        return asset.m_zone->m_script_strings[scrStr];
                                                    });

        return converter.Convert();
    }
} // namespace

namespace phys_preset
{
    void InfoStringDumperIW5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetPhysPreset::Type>& asset)
    {
        // Only dump raw when no gdt available
        if (context.m_gdt)
        {
            const auto infoString = CreateInfoString(asset);
            GdtEntry gdtEntry(asset.m_name, ObjConstants::GDF_FILENAME_PHYS_PRESET);
            infoString.ToGdtProperties(ObjConstants::INFO_STRING_PREFIX_PHYS_PRESET, gdtEntry);
            context.m_gdt->WriteEntry(gdtEntry);
        }
        else
        {
            const auto assetFile = context.OpenAssetFile(GetFileNameForAssetName(asset.m_name));

            if (!assetFile)
                return;

            auto& stream = *assetFile;
            const auto infoString = CreateInfoString(asset);
            const auto stringValue = infoString.ToString(ObjConstants::INFO_STRING_PREFIX_PHYS_PRESET);
            stream.write(stringValue.c_str(), stringValue.size());
        }
    }
} // namespace phys_preset
