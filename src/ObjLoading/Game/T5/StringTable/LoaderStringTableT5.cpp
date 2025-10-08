#include "LoaderStringTableT5.h"

#include "Game/T5/CommonT5.h"
#include "Game/T5/T5.h"
#include "StringTable/StringTableLoader.h"

#include <cstring>

using namespace T5;

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

            string_table::StringTableLoaderV3<StringTable, Common::Com_HashString> loader;
            auto* stringTable = loader.LoadFromStream(assetName, m_memory, *file.m_stream);

            return AssetCreationResult::Success(context.AddAsset<AssetStringTable>(assetName, stringTable));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace string_table
{
    std::unique_ptr<AssetCreator<AssetStringTable>> CreateLoaderT5(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<StringTableLoader>(memory, searchPath);
    }
} // namespace string_table
