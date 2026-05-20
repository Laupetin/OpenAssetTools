#include "InfoStringLoaderPhysPresetIW6.h"

#include "Game/IW6/IW6.h"
#include "Game/IW6/InfoString/EnumStrings.h"
#include "Game/IW6/InfoString/InfoStringToStructConverter.h"
#include "Game/IW6/PhysPreset/PhysPresetFields.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <cstring>
#include <limits>

using namespace IW6;

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
            switch (static_cast<physPresetFieldType_t>(field.iFieldType))
            {
            case PPFT_SCALING:
                return ConvertEnumInt(field.szName, value, field.iOffset, szPhysPresetScalingNames, std::extent_v<decltype(szPhysPresetScalingNames)>);

            default:
                assert(false);
                return false;
            }
        }
    };

    void CopyFromPhysPresetInfo(const PhysPresetInfo& physPresetInfo, PhysPreset& physPreset)
    {
        physPreset.mass = physPresetInfo.mass;
        physPreset.bounce = physPresetInfo.bounce;

        if (physPresetInfo.isFrictionInfinity != 0)
            physPreset.friction = std::numeric_limits<float>::max();
        else
            physPreset.friction = physPresetInfo.friction;

        physPreset.bulletForceScale = physPresetInfo.bulletForceScale;
        physPreset.explosiveForceScale = physPresetInfo.explosiveForceScale;
        physPreset.sndAliasPrefix = physPresetInfo.sndAliasPrefix;
        physPreset.piecesSpreadFraction = physPresetInfo.piecesSpreadFraction;
        physPreset.piecesUpwardVelocity = physPresetInfo.piecesUpwardVelocity;
        physPreset.minMomentum = physPresetInfo.minMomentum;
        physPreset.maxMomentum = physPresetInfo.maxMomentum;
        physPreset.minPitch = physPresetInfo.minPitch;
        physPreset.maxPitch = physPresetInfo.maxPitch;
        physPreset.volumeType = physPresetInfo.volumeType;
        physPreset.pitchType = physPresetInfo.pitchType;
        physPreset.tempDefaultToCylinder = physPresetInfo.tempDefaultToCylinder != 0;
        physPreset.perSurfaceSndAlias = physPresetInfo.perSurfaceSndAlias != 0;
    }
} // namespace

namespace phys_preset
{
    InfoStringLoaderIW6::InfoStringLoaderIW6(MemoryManager& memory, Zone& zone)
        : m_memory(memory),
          m_zone(zone)
    {
    }

    AssetCreationResult InfoStringLoaderIW6::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        PhysPresetInfo presetInfo;
        std::memset(&presetInfo, 0, sizeof(presetInfo));

        auto* physPreset = m_memory.Alloc<PhysPreset>();
        AssetRegistration<AssetPhysPreset> registration(assetName, physPreset);

        InfoStringToPhysPresetConverter converter(
            infoString, &presetInfo, m_zone.m_script_strings, m_memory, context, registration, phys_preset_fields, std::extent_v<decltype(phys_preset_fields)>);
        if (!converter.Convert())
        {
            con::error("Failed to parse phys preset: \"{}\"", assetName);
            return AssetCreationResult::Failure();
        }

        CopyFromPhysPresetInfo(presetInfo, *physPreset);
        physPreset->name = m_memory.Dup(assetName.c_str());

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace phys_preset
