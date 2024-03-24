#include "AssetLoaderZBarrier.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/InfoString/ZBarrierFields.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

#include <cassert>
#include <cstring>
#include <iostream>

using namespace T6;

namespace T6
{
    class InfoStringToZBarrierConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            assert(false);
            return false;
        }

    public:
        InfoStringToZBarrierConverter(const InfoString& infoString,
                                      ZBarrierDef* zbarrier,
                                      ZoneScriptStrings& zoneScriptStrings,
                                      MemoryManager* memory,
                                      IAssetLoadingManager* manager,
                                      const cspField_t* fields,
                                      const size_t fieldCount)
            : InfoStringToStructConverter(infoString, zbarrier, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
} // namespace T6

void AssetLoaderZBarrier::CalculateZBarrierFields(ZBarrierDef* zbarrier)
{
    // numBoardsInBarrier
    {
        auto foundEnd = false;
        for (auto i = 0u; i < std::extent_v<decltype(ZBarrierDef::boards)>; i++)
        {
            if (zbarrier->boards[i].pBoardModel == nullptr)
            {
                foundEnd = true;
                zbarrier->numBoardsInBarrier = i;
                break;
            }
        }

        if (!foundEnd)
            zbarrier->numBoardsInBarrier = std::extent_v<decltype(ZBarrierDef::boards)>;
    }
}

bool AssetLoaderZBarrier::LoadFromInfoString(
    const InfoString& infoString, const std::string& assetName, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone)
{
    auto* zbarrier = memory->Create<ZBarrierDef>();
    memset(zbarrier, 0, sizeof(ZBarrierDef));

    InfoStringToZBarrierConverter converter(
        infoString, zbarrier, zone->m_script_strings, memory, manager, zbarrier_fields, std::extent_v<decltype(zbarrier_fields)>);
    if (!converter.Convert())
    {
        std::cout << "Failed to parse zbarrier: \"" << assetName << "\"" << std::endl;
        return true;
    }

    CalculateZBarrierFields(zbarrier);
    zbarrier->name = memory->Dup(assetName.c_str());

    manager->AddAsset(ASSET_TYPE_ZBARRIER, assetName, zbarrier, converter.GetDependencies(), converter.GetUsedScriptStrings());

    return true;
}

void* AssetLoaderZBarrier::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* zbarrier = memory->Create<ZBarrierDef>();
    memset(zbarrier, 0, sizeof(ZBarrierDef));
    CalculateZBarrierFields(zbarrier);
    zbarrier->name = memory->Dup(assetName.c_str());
    return zbarrier;
}

bool AssetLoaderZBarrier::CanLoadFromGdt() const
{
    return true;
}

bool AssetLoaderZBarrier::LoadFromGdt(
    const std::string& assetName, IGdtQueryable* gdtQueryable, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    auto* gdtEntry = gdtQueryable->GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_ZBARRIER, assetName);
    if (gdtEntry == nullptr)
        return false;

    InfoString infoString;
    if (!infoString.FromGdtProperties(*gdtEntry))
    {
        std::cout << "Failed to read zbarrier gdt entry: \"" << assetName << "\"" << std::endl;
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}

bool AssetLoaderZBarrier::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderZBarrier::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = "zbarrier/" + assetName;
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    InfoString infoString;
    if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_ZBARRIER, *file.m_stream))
    {
        std::cout << "Failed to read zbarrier raw file: \"" << fileName << "\"" << std::endl;
        return true;
    }

    return LoadFromInfoString(infoString, assetName, memory, manager, zone);
}
