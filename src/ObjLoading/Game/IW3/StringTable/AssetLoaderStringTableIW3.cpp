#include "AssetLoaderStringTableIW3.h"

#include "Game/IW3/IW3.h"
#include "StringTable/StringTableLoader.h"

#include <cstring>

using namespace IW3;

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

            string_table::StringTableLoaderV1<StringTable> loader;
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

namespace IW3
{
    std::unique_ptr<AssetCreator<AssetStringTable>> CreateStringTableLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<StringTableLoader>(memory, searchPath);
    }
} // namespace IW3
