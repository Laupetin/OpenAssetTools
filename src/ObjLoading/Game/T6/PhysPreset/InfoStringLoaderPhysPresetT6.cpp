#include "InfoStringLoaderPhysPresetT6.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/PhysPreset/PhysPresetFields.h"
#include "Game/T6/T6.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace T6;

namespace
{
    class InfoStringToPhysPresetConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            assert(false);
            return false;
        }

    public:
        InfoStringToPhysPresetConverter(const InfoString& infoString,
                                        PhysPresetInfo& physPreset,
                                        ZoneScriptStrings& zoneScriptStrings,
                                        MemoryManager& memory,
                                        AssetCreationContext& context,
                                        AssetRegistration<AssetPhysPreset>& registration,
                                        const cspField_t* fields,
                                        const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &physPreset, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
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
        physPreset.piecesSpreadFraction = physPresetInfo.piecesSpreadFraction;
        physPreset.piecesUpwardVelocity = physPresetInfo.piecesUpwardVelocity;
        physPreset.canFloat = physPresetInfo.canFloat;
        physPreset.gravityScale = std::clamp(physPresetInfo.gravityScale, 0.01f, 10.0f);
        physPreset.centerOfMassOffset = physPresetInfo.centerOfMassOffset;
        physPreset.buoyancyBoxMin = physPresetInfo.buoyancyBoxMin;
        physPreset.buoyancyBoxMax = physPresetInfo.buoyancyBoxMax;
    }
} // namespace

namespace phys_preset
{
    InfoStringLoaderT6::InfoStringLoaderT6(MemoryManager& memory, Zone& zone)
        : m_memory(memory),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoaderT6::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        auto* physPreset = m_memory.Alloc<PhysPreset>();
        physPreset->name = m_memory.Dup(assetName.c_str());

        AssetRegistration<AssetPhysPreset> registration(assetName, physPreset);

        PhysPresetInfo physPresetInfo;
        memset(&physPresetInfo, 0, sizeof(physPresetInfo));
        InfoStringToPhysPresetConverter converter(infoString,
                                                  physPresetInfo,
                                                  m_zone.m_script_strings,
                                                  m_memory,
                                                  context,
                                                  registration,
                                                  phys_preset_fields,
                                                  std::extent_v<decltype(phys_preset_fields)>);
        if (!converter.Convert())
        {
            std::cerr << std::format("Failed to parse phys preset: \"{}\"\n", assetName);
            return AssetCreationResult::Failure();
        }

        CopyFromPhysPresetInfo(physPresetInfo, *physPreset);

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace phys_preset
