#include "InfoStringLoaderZBarrierT6.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/T6.h"
#include "Game/T6/ZBarrier/ZBarrierFields.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace T6;

namespace
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
                                      ZBarrierDef& zbarrier,
                                      ZoneScriptStrings& zoneScriptStrings,
                                      MemoryManager& memory,
                                      AssetCreationContext& context,
                                      AssetRegistration<AssetZBarrier>& registration,
                                      const cspField_t* fields,
                                      const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &zbarrier, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };

    void CalculateZBarrierFields(ZBarrierDef& zbarrier)
    {
        auto foundEnd = false;
        for (auto i = 0u; i < std::extent_v<decltype(ZBarrierDef::boards)>; i++)
        {
            if (zbarrier.boards[i].pBoardModel == nullptr)
            {
                foundEnd = true;
                zbarrier.numBoardsInBarrier = i;
                break;
            }
        }

        if (!foundEnd)
            zbarrier.numBoardsInBarrier = std::extent_v<decltype(ZBarrierDef::boards)>;
    }
} // namespace

InfoStringLoaderZBarrier::InfoStringLoaderZBarrier(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    : m_memory(memory),
      m_search_path(searchPath),
      m_zone(zone)
{
}

AssetCreationResult InfoStringLoaderZBarrier::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
{
    auto* zbarrier = m_memory.Alloc<ZBarrierDef>();
    zbarrier->name = m_memory.Dup(assetName.c_str());

    AssetRegistration<AssetZBarrier> registration(assetName, zbarrier);

    InfoStringToZBarrierConverter converter(
        infoString, *zbarrier, m_zone.m_script_strings, m_memory, context, registration, zbarrier_fields, std::extent_v<decltype(zbarrier_fields)>);
    if (!converter.Convert())
    {
        std::cerr << std::format("Failed to parse zbarrier: \"{}\"\n", assetName);
        return AssetCreationResult::Failure();
    }

    CalculateZBarrierFields(*zbarrier);

    return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
}
