#include "WeaponDumperIW3.h"

#include "WeaponWriterIW3.h"
#include "ObjWriting.h"

#include <filesystem>
#include <string>
#include <Weapon/WeaponDumpingZoneState.h>

using namespace IW3;

namespace
{
    std::string GetPathForWeapon(weapon::WeaponDumpingZoneState* zoneState, const XAssetInfo<Weapon>& asset)
    {
        const auto weaponDumpingState = zoneState->m_weapon_dumping_state_map.find(asset.Asset());

        if (weaponDumpingState == zoneState->m_weapon_dumping_state_map.end())
            return "weapons/" + std::string(asset.Asset()->name);

        return physPresetDumpingState->second.m_path;
    }
} // namespace

namespace weapon
{
    void WeaponDumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetWeapon::Type>& asset)
    {
        const auto* weapon = asset.Asset();
        auto* zoneState = context.GetZoneAssetDumperState<WeaponDumpingZoneState>();

        const auto weaponPath = GetPathForWeapon(zoneState, asset);
        const auto assetFile = context.OpenAssetFile(weaponPath);

        if (!assetFile)
            return;

        auto weaponWriter = CreateWeaponWriterIW3(*assetFile);

        weaponWriter->WritePhysPreset(*weapon);
    }
} // namespace physpreset
