#include "FxImpactTableLoaderIW3.h"

#include "FxImpactTable/FxImpactTableCommon.h"
#include "Game/IW3/IW3.h"
#include "Utils/Logging/Log.h"

#include <Csv/CsvStream.h>
#include <Game/IW3/Fx/FxImpactTableConstantsIW3.h>
#include <format>
#include <iostream>

using namespace IW3;

namespace
{
    constexpr unsigned COL_COUNT_MIN = 3;
    constexpr unsigned NON_FLESH_COUNT = std::extent_v<decltype(fx_nonflesh_surface_type_names)>;
    constexpr unsigned FLESH_COUNT = std::extent_v<decltype(fx_flesh_surface_type_names)>;
    constexpr unsigned SURFACE_COUNT = NON_FLESH_COUNT + FLESH_COUNT;
    constexpr unsigned IMPACT_COUNT = std::extent_v<decltype(fx_impact_type_names)>;
    constexpr unsigned ENTRY_COUNT = IMPACT_COUNT * (NON_FLESH_COUNT + FLESH_COUNT);

    class FxImpactTableLoader : public AssetCreator<AssetImpactFx>
    {
    public:
        FxImpactTableLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = fx_impact_table::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* fxImpactTable = m_memory.Alloc<FxImpactTable>();
            fxImpactTable->name = m_memory.Dup(assetName.c_str());
            fxImpactTable->table = m_memory.Alloc<FxImpactEntry>(IMPACT_COUNT);
            AssetRegistration<AssetImpactFx> registration(assetName, fxImpactTable);

            for (size_t i = 0; i < IMPACT_COUNT; ++i)
            {
                auto& entry = fxImpactTable->table[i];

                for (auto& ptr : entry.nonflesh)
                    ptr = nullptr;

                for (auto& ptr : entry.flesh)
                    ptr = nullptr;
            }

            const CsvInputStream csv(*file.m_stream);
            std::vector<std::string> currentRow;

            auto currentRowIndex = 0u;
            auto dataRowIndex = 0u;
            while (csv.NextRow(currentRow))
            {
                currentRowIndex++;
                CsvInputStream::PreprocessRow(currentRow);

                if (CsvInputStream::RowIsEmpty(currentRow))
                    continue;

                if (currentRow.size() < COL_COUNT_MIN)
                {
                    con::error("{} Column count lower than min column count ({})", ErrorPrefix(assetName, currentRowIndex), COL_COUNT_MIN);
                    return AssetCreationResult::Failure();
                }

                const std::string& name = currentRow[2];
                if (name.empty())
                {
                    continue;
                }

                const size_t rowIndex = dataRowIndex++;
                if (rowIndex >= ENTRY_COUNT)
                {
                    con::warn("Found extra entry #{} in FxImpactTable!", currentRowIndex);
                    continue;
                }

                const size_t impactIndex = rowIndex / SURFACE_COUNT;
                const size_t surfaceIndex = rowIndex % SURFACE_COUNT;
                auto& entry = fxImpactTable->table[impactIndex];

                auto makeFx = [&]() -> FxEffectDef*
                {
                    if (name.empty())
                        return nullptr;

                    return context.ForceLoadDependency<AssetFx>(name)->Asset();
                };

                if (surfaceIndex < NON_FLESH_COUNT)
                {
                    entry.nonflesh[surfaceIndex] = makeFx();
                }
                else
                {
                    entry.flesh[surfaceIndex - NON_FLESH_COUNT] = makeFx();
                }
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        static std::string ErrorPrefix(const std::string& assetName, const unsigned rowIndex)
        {
            return std::format("FxImpactTable \"{}\" Row {}:", assetName, rowIndex);
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace fx_impact_table
{
    std::unique_ptr<AssetCreator<AssetImpactFx>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<FxImpactTableLoader>(memory, searchPath, zone);
    }
} // namespace fx_impact_table
