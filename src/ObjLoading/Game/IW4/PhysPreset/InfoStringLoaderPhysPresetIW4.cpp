#include "InfoStringLoaderPhysPresetIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/InfoString/InfoStringToStructConverter.h"
#include "Game/IW4/PhysPreset/PhysPresetFields.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace IW4;

namespace
{
    class InfoStringToPhysPresetConverter final : public InfoStringToStructConverter
    {
    public:
        InfoStringToPhysPresetConverter(const InfoString& infoString,
                                        void* structure,
                                        ZoneScriptStrings& zoneScriptStrings,
                                        MemoryManager& memory,
                                        AssetCreationContext& context,
                                        GenericAssetRegistration& registration,
                                        const cspField_t* fields,
                                        size_t fieldCount)
            : InfoStringToStructConverter(infoString, structure, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }

    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            assert(false);
            return false;
        }
    };

    void CopyFromPhysPresetInfo(const PhysPresetInfo& physPresetInfo, PhysPreset& physPreset)
    {
        physPreset.mass = std::clamp(physPresetInfo.mass, 1.0f, 2000.0f) * 0.001f;
        physPreset.bounce = physPresetInfo.bounce;

        if (physPresetInfo.isFrictionInfinity != 0)
            physPreset.friction = std::numeric_limits<float>::infinity();
        else
            physPreset.friction = physPresetInfo.friction;

        physPreset.bulletForceScale = physPresetInfo.bulletForceScale;
        physPreset.explosiveForceScale = physPresetInfo.explosiveForceScale;
        physPreset.sndAliasPrefix = physPresetInfo.sndAliasPrefix;
        physPreset.piecesSpreadFraction = physPresetInfo.piecesSpreadFraction;
        physPreset.piecesUpwardVelocity = physPresetInfo.piecesUpwardVelocity;
        physPreset.tempDefaultToCylinder = physPresetInfo.tempDefaultToCylinder != 0;
        physPreset.perSurfaceSndAlias = physPresetInfo.perSurfaceSndAlias != 0;
    }
} // namespace

namespace phys_preset
{
    InfoStringLoaderIW4::InfoStringLoaderIW4(MemoryManager& memory, Zone& zone)
        : m_memory(memory),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoaderIW4::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        PhysPresetInfo presetInfo;
        std::memset(&presetInfo, 0, sizeof(presetInfo));

        auto* physPreset = m_memory.Alloc<PhysPreset>();
        AssetRegistration<AssetPhysPreset> registration(assetName, physPreset);

        InfoStringToPhysPresetConverter converter(
            infoString, &presetInfo, m_zone.m_script_strings, m_memory, context, registration, phys_preset_fields, std::extent_v<decltype(phys_preset_fields)>);
        if (!converter.Convert())
        {
            std::cerr << std::format("Failed to parse phys preset: \"{}\"\n", assetName);
            return AssetCreationResult::Failure();
        }

        CopyFromPhysPresetInfo(presetInfo, *physPreset);
        physPreset->name = m_memory.Dup(assetName.c_str());

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace phys_preset
