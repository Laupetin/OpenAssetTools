#include "PhysPresetDumperIW3.h"

#include "PhysPresetWriterIW3.h"
#include "ObjWriting.h"

#include <filesystem>
#include <string>
#include <PhysPreset/PhysPresetDumpingZoneState.h>

using namespace IW3;

namespace
{
    std::string GetPathForPhysPreset(physpreset::PhysPresetDumpingZoneState* zoneState, const XAssetInfo<PhysPreset>& asset)
    {
        const auto physPresetDumpingState = zoneState->m_physpreset_dumping_state_map.find(asset.Asset());

        if (physPresetDumpingState == zoneState->m_physpreset_dumping_state_map.end())
            return "physic/" + std::string(asset.Asset()->name);

        return physPresetDumpingState->second.m_path;
    }
} // namespace

namespace physpreset
{
    void PhysPresetDumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetPhysPreset::Type>& asset)
    {
        const auto* physPreset = asset.Asset();
        auto* zoneState = context.GetZoneAssetDumperState<PhysPresetDumpingZoneState>();

        const auto physPresetPath = GetPathForPhysPreset(zoneState, asset);
        const auto assetFile = context.OpenAssetFile(physPresetPath);

        if (!assetFile)
            return;

        auto physPresetWriter = CreatePhysPresetWriterIW3(*assetFile);

        physPresetWriter->WritePhysPreset(*physPreset);
    }
} // namespace physpreset
