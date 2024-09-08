#include "AssetLoaderPhysConstraints.h"

#include "Game/T6/InfoString/EnumStrings.h"
#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/InfoString/PhysConstraintsFields.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

#include <cassert>
#include <cstring>
#include <iostream>

using namespace T6;

namespace T6
{
    class InfoStringToPhysConstraintsConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<constraintsFieldType_t>(field.iFieldType))
            {
            case CFT_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, s_constraintTypeNames, std::extent_v<decltype(s_constraintTypeNames)>);

            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToPhysConstraintsConverter(const InfoString& infoString,
                                             PhysConstraints* physConstraints,
                                             ZoneScriptStrings& zoneScriptStrings,
                                             MemoryManager* memory,
                                             IAssetLoadingManager* manager,
                                             const cspField_t* fields,
                                             const size_t fieldCount)
            : InfoStringToStructConverter(infoString, physConstraints, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
} // namespace T6

void AssetLoaderPhysConstraints::CalculatePhysConstraintsFields(PhysConstraints* physConstraints, Zone* zone)
{
    for (auto& constraint : physConstraints->data)
    {
        constraint.targetname = zone->m_script_strings.AddOrGetScriptString("");
        constraint.target_ent1 = zone->m_script_strings.AddOrGetScriptString("");
        constraint.target_ent2 = zone->m_script_strings.AddOrGetScriptString("");
        constraint.attach_point_type1 = ATTACH_POINT_BONE;
        constraint.attach_point_type2 = ATTACH_POINT_BONE;
        constraint.target_bone1 = "";
        constraint.target_bone2 = "";
    }

    // count
    {
        auto foundEnd = false;
        for (auto i = 0u; i < std::extent_v<decltype(PhysConstraints::data)>; i++)
        {
            if (physConstraints->data[i].type == CONSTRAINT_NONE)
            {
                foundEnd = true;
                physConstraints->count = i;
                break;
            }
        }

        if (!foundEnd)
            physConstraints->count = std::extent_v<decltype(PhysConstraints::data)>;
    }
}

bool AssetLoaderPhysConstraints::LoadFromInfoString(
    const InfoString& infoString, const std::string& assetName, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone)
{
    auto* physConstraints = memory->Create<PhysConstraints>();
    memset(physConstraints, 0, sizeof(PhysConstraints));

    InfoStringToPhysConstraintsConverter converter(
        infoString, physConstraints, zone->m_script_strings, memory, manager, phys_constraints_fields, std::extent_v<decltype(phys_constraints_fields)>);
    if (!converter.Convert())
    {
        std::cout << "Failed to parse phys constraints: \"" << assetName << "\"\n";
        return true;
    }

    CalculatePhysConstraintsFields(physConstraints, zone);
    physConstraints->name = memory->Dup(assetName.c_str());

    auto scrStrings = converter.GetUsedScriptStrings();
    scrStrings.push_back(zone->m_script_strings.AddOrGetScriptString(""));
    manager->AddAsset<AssetPhysConstraints>(assetName, physConstraints, converter.GetDependencies(), scrStrings);

    return true;
}

void* AssetLoaderPhysConstraints::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetPhysConstraints::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}

bool AssetLoaderPhysConstraints::CanLoadFromGdt() const
{
    return true;
}

bool AssetLoaderPhysConstraints::LoadFromGdt(
    const std::string& assetName, IGdtQueryable* gdtQueryable, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    auto* gdtEntry = gdtQueryable->GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_PHYS_CONSTRAINTS, assetName);
    if (gdtEntry == nullptr)
        return false;

    InfoString infoString;
    if (!infoString.FromGdtProperties(*gdtEntry))
    {
        std::cout << "Failed to read phys constraints gdt entry: \"" << assetName << "\"\n";
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}

bool AssetLoaderPhysConstraints::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderPhysConstraints::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "physconstraints/" + assetName;
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_PHYS_CONSTRAINTS, *file.m_stream))
    {
        std::cerr << "Could not parse as info string file: \"" << fileName << "\"\n";
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}
