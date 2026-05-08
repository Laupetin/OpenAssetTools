#include "LoaderSndAliasListIW3.h"

#include "Game/IW3/IW3.h"
#include "Sound/SndAliasListCommon.h"
#include "Utils/Logging/Log.h"

#include <Csv/CsvStream.h>
#include <format>
#include <iostream>

using namespace IW3;

namespace
{
    constexpr unsigned COL_COUNT_MIN = 29;

    class SndAliasListLoader : public AssetCreator<AssetSound>
    {
    public:
        SndAliasListLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = snd_alias::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* aliasList = m_memory.Alloc<snd_alias_list_t>();
            aliasList->aliasName = m_memory.Dup(assetName.c_str());
            aliasList->head = m_memory.Alloc<snd_alias_t>(aliasList->count);
            AssetRegistration<AssetSound> registration(assetName, aliasList);

            const CsvInputStream csv(*file.m_stream);
            std::vector<std::string> currentRow;
            std::vector<snd_alias_t> aliases;

            // Skip header
            csv.NextRow(currentRow);

            auto currentRowIndex = 0u;
            auto dataRowIndex = 0u;
            while (csv.NextRow(currentRow))
            {
                snd_alias_t alias;

                currentRowIndex++;
                CsvInputStream::PreprocessRow(currentRow);

                if (CsvInputStream::RowIsEmpty(currentRow))
                    continue;

                if (currentRow.size() < COL_COUNT_MIN)
                {
                    con::error("{} Column count lower than min column count ({})", ErrorPrefix(assetName, currentRowIndex), COL_COUNT_MIN);
                    return AssetCreationResult::Failure();
                }

                if (!currentRow[0].empty())
                {
                    alias.aliasName = currentRow[0].c_str();
                }

                int sequence = std::stoi(currentRow[1]);
                if (sequence == 0)
                {
                    alias.sequence = currentRowIndex - 1;
                }
                else
                {
                    alias.sequence = sequence;
                }

                // auto makeFx = [&]() -> FxEffectDef*
                //{
                //     if (name.empty())
                //         return nullptr;

                //    return context.ForceLoadDependency<AssetFx>(name)->Asset();
                //};

                aliases.push_back(alias);
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        static std::string ErrorPrefix(const std::string& assetName, const unsigned rowIndex)
        {
            return std::format("SoundAliasList \"{}\" Row {}:", assetName, rowIndex);
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace snd_alias_list
{
    std::unique_ptr<AssetCreator<AssetSound>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<SndAliasListLoader>(memory, searchPath, zone);
    }
} // namespace snd_alias_list
