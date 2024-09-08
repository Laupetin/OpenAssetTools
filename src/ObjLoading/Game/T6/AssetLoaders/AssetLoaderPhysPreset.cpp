#include "AssetLoaderPhysPreset.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/InfoString/PhysPresetFields.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>

using namespace T6;

namespace T6
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
                                        PhysPresetInfo* physPreset,
                                        ZoneScriptStrings& zoneScriptStrings,
                                        MemoryManager* memory,
                                        IAssetLoadingManager* manager,
                                        const cspField_t* fields,
                                        const size_t fieldCount)
            : InfoStringToStructConverter(infoString, physPreset, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
} // namespace T6

void AssetLoaderPhysPreset::CopyFromPhysPresetInfo(const PhysPresetInfo* physPresetInfo, PhysPreset* physPreset)
{
    physPreset->mass = std::clamp(physPresetInfo->mass, 1.0f, 2000.0f) * 0.001f;
    physPreset->bounce = physPresetInfo->bounce;

    if (physPresetInfo->isFrictionInfinity != 0)
        physPreset->friction = std::numeric_limits<float>::infinity();
    else
        physPreset->friction = physPresetInfo->friction;

    physPreset->bulletForceScale = physPresetInfo->bulletForceScale;
    physPreset->explosiveForceScale = physPresetInfo->explosiveForceScale;
    physPreset->piecesSpreadFraction = physPresetInfo->piecesSpreadFraction;
    physPreset->piecesUpwardVelocity = physPresetInfo->piecesUpwardVelocity;
    physPreset->canFloat = physPresetInfo->canFloat;
    physPreset->gravityScale = std::clamp(physPresetInfo->gravityScale, 0.01f, 10.0f);
    physPreset->centerOfMassOffset = physPresetInfo->centerOfMassOffset;
    physPreset->buoyancyBoxMin = physPresetInfo->buoyancyBoxMin;
    physPreset->buoyancyBoxMax = physPresetInfo->buoyancyBoxMax;
}

bool AssetLoaderPhysPreset::LoadFromInfoString(
    const InfoString& infoString, const std::string& assetName, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone)
{
    const auto presetInfo = std::make_unique<PhysPresetInfo>();
    memset(presetInfo.get(), 0, sizeof(PhysPresetInfo));
    InfoStringToPhysPresetConverter converter(
        infoString, presetInfo.get(), zone->m_script_strings, memory, manager, phys_preset_fields, std::extent_v<decltype(phys_preset_fields)>);
    if (!converter.Convert())
    {
        std::cout << "Failed to parse phys preset: \"" << assetName << "\"\n";
        return true;
    }

    auto* physPreset = memory->Create<PhysPreset>();

    CopyFromPhysPresetInfo(presetInfo.get(), physPreset);
    physPreset->name = memory->Dup(assetName.c_str());

    manager->AddAsset<AssetPhysPreset>(assetName, physPreset, converter.GetDependencies(), converter.GetUsedScriptStrings());

    return true;
}

void* AssetLoaderPhysPreset::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetPhysPreset::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}

bool AssetLoaderPhysPreset::CanLoadFromGdt() const
{
    return true;
}

bool AssetLoaderPhysPreset::LoadFromGdt(
    const std::string& assetName, IGdtQueryable* gdtQueryable, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    auto* gdtEntry = gdtQueryable->GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_PHYS_PRESET, assetName);
    if (gdtEntry == nullptr)
        return false;

    InfoString infoString;
    if (!infoString.FromGdtProperties(*gdtEntry))
    {
        std::cout << "Failed to read phys preset gdt entry: \"" << assetName << "\"\n";
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}

bool AssetLoaderPhysPreset::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderPhysPreset::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "physic/" + assetName;
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_PHYS_PRESET, *file.m_stream))
    {
        std::cerr << "Could not parse as info string file: \"" << fileName << "\"\n";
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}
