#include "LoaderStringTableIW4.h"

#include "Csv/CsvStream.h"
#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/IW4.h"
#include "StringTable/StringTableLoader.h"

using namespace IW4;
using namespace ::string_table;

namespace
{
    class LoaderStringTable final : public AssetCreator<AssetStringTable>
    {
    public:
        LoaderStringTable(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(assetName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            StringTableLoaderV2<StringTable, Common::StringTable_HashString> loader;
            auto* stringTable = loader.LoadFromStream(assetName, m_memory, *file.m_stream);
            if (!stringTable)
                return AssetCreationResult::Failure();

            return AssetCreationResult::Success(context.AddAsset<AssetStringTable>(assetName, stringTable));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW4::string_table
{
    std::unique_ptr<AssetCreator<AssetStringTable>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<LoaderStringTable>(memory, searchPath);
    }
} // namespace IW4::string_table
