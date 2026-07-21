#include "PhysPresetInfoStringDumperIW4.h"

#include "Game/IW4/InfoString/InfoStringFromStructConverter.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "Game/IW4/PhysPreset/PhysPresetFields.h"
#include "PhysPreset/PhysPresetCommon.h"

#include <cassert>
#include <limits>
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
        InfoStringFromPhysPresetConverter(const PhysPresetInfo* structure, const cspField_t* fields, const size_t fieldCount)
            : InfoStringFromStructConverter(structure, fields, fieldCount)
        {
        }
    };

    void CopyToPhysPresetInfo(const PhysPreset& physPreset, PhysPresetInfo& physPresetInfo)
    {
        physPresetInfo.mass = physPreset.mass;
        physPresetInfo.bounce = physPreset.bounce;

        if (physPreset.friction >= std::numeric_limits<float>::max())
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
        physPresetInfo.tempDefaultToCylinder = physPreset.tempDefaultToCylinder ? 1 : 0;
        physPresetInfo.perSurfaceSndAlias = physPreset.perSurfaceSndAlias ? 1 : 0;
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
    void InfoStringDumperIW4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetPhysPreset::Type>& asset)
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
