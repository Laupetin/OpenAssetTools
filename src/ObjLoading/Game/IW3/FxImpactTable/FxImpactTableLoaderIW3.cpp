#include "FxImpactTableLoaderIW3.h"

#include "FxImpactTable/FxImpactTableCommon.h"
#include "Game/IW3/IW3.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>
#include <Csv/CsvStream.h>
#include <Game/IW3/FxImpactTable/FxImpactTableFields.h>

using namespace IW3;

namespace
{
    constexpr unsigned COL_COUNT_MIN = 3;

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

            size_t entryCount = std::extent_v<decltype(fx_impact_type_names)>
                                * (std::extent_v<decltype(fx_nonflesh_surface_type_names)> 
                                    + std::extent_v<decltype(fx_flesh_surface_type_names)>);

            auto* fxImpactTable = m_memory.Alloc<FxImpactTable>();
            fxImpactTable->name = m_memory.Dup(assetName.c_str());
            fxImpactTable->table = m_memory.Alloc<FxImpactEntry>(entryCount);
            AssetRegistration<AssetImpactFx> registration(assetName, fxImpactTable);

            const CsvInputStream csv(*file.m_stream);
            std::vector<std::string> currentRow;
            std::vector<FxImpactEntry> entries;
            auto currentRowIndex = 0u;

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

                if (currentRowIndex - 1 < std::extent_v<decltype(fx_nonflesh_surface_type_names)>)
                {
                    fxImpactTable->table->nonflesh[currentRowIndex - 1]->name = currentRow[2].c_str();
                }
                else
                {
                    fxImpactTable->table->flesh[currentRowIndex - 1]->name = currentRow[2].c_str();
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
