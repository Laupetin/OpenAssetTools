#include "AssetLoaderPhysPreset.h"

#include <cstring>
#include <iostream>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "Game/IW4/InfoString/InfoStringToStructConverter.h"
#include "Game/IW4/InfoString/PhysPresetFields.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

namespace IW4
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
        InfoStringToPhysPresetConverter(const InfoString& infoString, PhysPresetInfo* physPreset, ZoneScriptStrings& zoneScriptStrings, MemoryManager* memory, IAssetLoadingManager* manager,
            const cspField_t* fields, const size_t fieldCount)
            : InfoStringToStructConverter(infoString, physPreset, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
}

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
    physPreset->sndAliasPrefix = physPresetInfo->sndAliasPrefix;
    physPreset->piecesSpreadFraction = physPresetInfo->piecesSpreadFraction;
    physPreset->piecesUpwardVelocity = physPresetInfo->piecesUpwardVelocity;
    physPreset->tempDefaultToCylinder = physPresetInfo->tempDefaultToCylinder != 0;
    physPreset->perSurfaceSndAlias = physPresetInfo->perSurfaceSndAlias != 0;
}

bool AssetLoaderPhysPreset::LoadFromInfoString(const InfoString& infoString, const std::string& assetName, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone)
{
    const auto presetInfo = std::make_unique<PhysPresetInfo>();
    memset(presetInfo.get(), 0, sizeof(PhysPresetInfo));
    InfoStringToPhysPresetConverter converter(infoString, presetInfo.get(), zone->m_script_strings, memory, manager, phys_preset_fields, std::extent<decltype(phys_preset_fields)>::value);
    if (!converter.Convert())
    {
        std::cout << "Failed to parse phys preset: \"" << assetName << "\"" << std::endl;
        return true;
    }

    auto* physPreset = memory->Create<PhysPreset>();

    CopyFromPhysPresetInfo(presetInfo.get(), physPreset);
    physPreset->name = memory->Dup(assetName.c_str());

    manager->AddAsset(ASSET_TYPE_PHYSPRESET, assetName, physPreset, converter.GetDependencies(), converter.GetUsedScriptStrings());

    return true;
}

void* AssetLoaderPhysPreset::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* physPreset = memory->Create<PhysPreset>();
    memset(physPreset, 0, sizeof(PhysPreset));
    physPreset->name = memory->Dup(assetName.c_str());
    return physPreset;
}

bool AssetLoaderPhysPreset::CanLoadFromGdt() const
{
    return true;
}

bool AssetLoaderPhysPreset::LoadFromGdt(const std::string& assetName, IGdtQueryable* gdtQueryable, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    auto* gdtEntry = gdtQueryable->GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_PHYS_PRESET, assetName);
    if (gdtEntry == nullptr)
        return false;

    InfoString infoString;
    if (!infoString.FromGdtProperties(*gdtEntry))
    {
        std::cout << "Failed to read phys preset gdt entry: \"" << assetName << "\"" << std::endl;
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}

bool AssetLoaderPhysPreset::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderPhysPreset::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "physic/" + assetName;
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_PHYS_PRESET, *file.m_stream))
    {
        std::cout << "Failed to read phys preset raw file: \"" << fileName << "\"" << std::endl;
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}

