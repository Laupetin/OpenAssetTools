#include "PhysPresetInfoStringDumperIW4.h"

#include "Game/IW4/InfoString/InfoStringFromStructConverter.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "Game/IW4/PhysPreset/PhysPresetFields.h"
#include "PhysPreset/PhysPresetCommon.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

using namespace IW4;

namespace
{
    class InfoStringFromPhysPresetConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            assert(false);
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
        physPresetInfo->mass = std::clamp(physPreset->mass * 1000.0f, 1.0f, 2000.0f);
        physPresetInfo->bounce = physPreset->bounce;

        if (std::isinf(physPreset->friction))
        {
            physPresetInfo->isFrictionInfinity = 1;
            physPresetInfo->friction = 0;
        }
        else
        {
            physPresetInfo->isFrictionInfinity = 0;
            physPresetInfo->friction = physPreset->friction;
        }

        physPresetInfo->bulletForceScale = physPreset->bulletForceScale;
        physPresetInfo->explosiveForceScale = physPreset->explosiveForceScale;
        physPresetInfo->sndAliasPrefix = physPreset->sndAliasPrefix;
        physPresetInfo->piecesSpreadFraction = physPreset->piecesSpreadFraction;
        physPresetInfo->piecesUpwardVelocity = physPreset->piecesUpwardVelocity;
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
    InfoStringDumperIW4::InfoStringDumperIW4(const AssetPool<AssetPhysPreset::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void InfoStringDumperIW4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetPhysPreset::Type>& asset)
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
