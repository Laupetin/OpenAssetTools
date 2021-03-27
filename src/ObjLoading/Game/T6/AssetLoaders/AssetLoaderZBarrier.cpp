#include "AssetLoaderZBarrier.h"

#include <cstring>
#include <iostream>
#include <cassert>

#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/InfoString/ZBarrierFields.h"
#include "InfoString/InfoString.h"

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
        InfoStringToZBarrierConverter(const InfoString& infoString, ZBarrierDef* zbarrier, ZoneScriptStrings& zoneScriptStrings, MemoryManager* memory, IAssetLoadingManager* manager,
            const cspField_t* fields, const size_t fieldCount)
            : InfoStringToStructConverter(infoString, zbarrier, zoneScriptStrings, memory, manager, fields, fieldCount)
        {
        }
    };
}

void AssetLoaderZBarrier::CalculateZBarrierFields(ZBarrierDef* zbarrier)
{
    // numBoardsInBarrier
    {
        auto foundEnd = false;
        for (auto i = 0u; i < std::extent<decltype(ZBarrierDef::boards)>::value; i++)
        {
            if (zbarrier->boards[i].pBoardModel == nullptr)
            {
                foundEnd = true;
                zbarrier->numBoardsInBarrier = i;
            }
        }

        if (!foundEnd)
            zbarrier->numBoardsInBarrier = std::extent<decltype(ZBarrierDef::boards)>::value;
    }
}

void* AssetLoaderZBarrier::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* zbarrier = memory->Create<ZBarrierDef>();
    memset(zbarrier, 0, sizeof(ZBarrierDef));
    CalculateZBarrierFields(zbarrier);
    zbarrier->name = memory->Dup(assetName.c_str());
    return zbarrier;
}

bool AssetLoaderZBarrier::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderZBarrier::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
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

    auto* zbarrier = memory->Create<ZBarrierDef>();
    memset(zbarrier, 0, sizeof(ZBarrierDef));

    InfoStringToZBarrierConverter converter(infoString, zbarrier, zone->m_script_strings, memory, manager, zbarrier_fields, std::extent<decltype(zbarrier_fields)>::value);
    if (!converter.Convert())
    {
        std::cout << "Failed to parse zbarrier raw file: \"" << fileName << "\"" << std::endl;
        return true;
    }

    CalculateZBarrierFields(zbarrier);
    zbarrier->name = memory->Dup(assetName.c_str());

    manager->AddAsset(ASSET_TYPE_ZBARRIER, assetName, zbarrier, converter.GetDependencies(), converter.GetUsedScriptStrings());

    return true;
}
