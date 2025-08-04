#include "LoaderStringTableIW5.h"

#include "Csv/CsvStream.h"
#include "Game/IW5/CommonIW5.h"
#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"
#include "StringTable/StringTableLoader.h"

#include <cstring>

using namespace IW5;
using namespace ::string_table;

namespace
{
    class StringTableLoader final : public AssetCreator<AssetStringTable>
    {
    public:
        StringTableLoader(MemoryManager& memory, ISearchPath& searchPath)
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

            return AssetCreationResult::Success(context.AddAsset<AssetStringTable>(assetName, stringTable));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW5::string_table
{
    std::unique_ptr<AssetCreator<AssetStringTable>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<StringTableLoader>(memory, searchPath);
    }
} // namespace IW5::string_table
