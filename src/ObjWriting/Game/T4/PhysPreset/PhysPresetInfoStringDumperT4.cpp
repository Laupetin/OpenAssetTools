#include "PhysPresetInfoStringDumperT4.h"

#include "Game/T4/InfoString/InfoStringFromStructConverter.h"
#include "Game/T4/ObjConstantsT4.h"
#include "Game/T4/PhysPreset/PhysPresetFields.h"
#include "PhysPreset/PhysPresetCommon.h"

#include <algorithm>
#include <cassert>
#include <type_traits>

using namespace T4;

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
        InfoStringFromPhysPresetConverter(const PhysPresetInfo* structure, const cspField_t* fields, const size_t fieldCount)
            : InfoStringFromStructConverter(structure, fields, fieldCount)
        {
        }
    };

    void CopyToPhysPresetInfo(const PhysPreset& physPreset, PhysPresetInfo& physPresetInfo)
    {
        physPresetInfo.mass = std::clamp(physPreset.mass * 1000.0f, 1.0f, 2000.0f);
        physPresetInfo.bounce = physPreset.bounce;

        if (physPreset.friction >= MAX_FRICTION)
        {
            physPresetInfo.isFrictionInfinity = 1;
            physPresetInfo.friction = 0;
        }
        else
        {
            physPresetInfo.isFrictionInfinity = 0;
            physPresetInfo.friction = physPreset.friction;
        }

        physPresetInfo.bulletForceScale = physPreset.bulletForceScale;
        physPresetInfo.explosiveForceScale = physPreset.explosiveForceScale;
        physPresetInfo.sndAliasPrefix = physPreset.sndAliasPrefix;
        physPresetInfo.piecesSpreadFraction = physPreset.piecesSpreadFraction;
        physPresetInfo.piecesUpwardVelocity = physPreset.piecesUpwardVelocity;
        physPresetInfo.canFloat = physPreset.canFloat;
        physPresetInfo.gravityScale = std::clamp(physPreset.gravityScale, 0.01f, 10.0f);
    }

    InfoString CreateInfoString(const PhysPreset& physPreset)
    {
        PhysPresetInfo physPresetInfo{};
        CopyToPhysPresetInfo(physPreset, physPresetInfo);

        InfoStringFromPhysPresetConverter converter(&physPresetInfo, phys_preset_fields, std::extent_v<decltype(phys_preset_fields)>);
        return converter.Convert();
    }
} // namespace

namespace phys_preset
{
    void InfoStringDumperT4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetPhysPreset::Type>& asset)
    {
        // Only dump raw when no gdt available
        if (context.m_gdt)
        {
            const auto infoString = CreateInfoString(*asset.Asset());
            GdtEntry gdtEntry(asset.m_name, GDF_FILENAME_PHYS_PRESET);
            infoString.ToGdtProperties(INFO_STRING_PREFIX_PHYS_PRESET, gdtEntry);
            context.m_gdt->WriteEntry(gdtEntry);
        }
        else
        {
            const auto assetFile = context.OpenAssetFile(GetFileNameForAssetName(asset.m_name));

            if (!assetFile)
                return;

            auto& stream = *assetFile;
            const auto infoString = CreateInfoString(*asset.Asset());
            const auto stringValue = infoString.ToString(INFO_STRING_PREFIX_PHYS_PRESET);
            stream.write(stringValue.c_str(), stringValue.size());
        }
    }
} // namespace phys_preset
