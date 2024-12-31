#include "LoaderSlugT6.h"

#include "Game/T6/T6.h"

#include <cstring>

using namespace T6;

namespace
{
    class SlugLoader final : public AssetCreator<AssetSlug>
    {
    public:
        SlugLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(assetName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* slug = m_memory.Alloc<Slug>();
            slug->name = m_memory.Dup(assetName.c_str());
            slug->len = static_cast<int>(file.m_length);

            auto* fileBuffer = m_memory.Alloc<char>(static_cast<size_t>(file.m_length + 1));
            file.m_stream->read(fileBuffer, file.m_length);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();
            fileBuffer[slug->len] = '\0';

            slug->buffer = static_cast<char16*>(fileBuffer);

            return AssetCreationResult::Success(context.AddAsset<AssetSlug>(assetName, slug));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace T6
{
    std::unique_ptr<AssetCreator<AssetSlug>> CreateSlugLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<SlugLoader>(memory, searchPath);
    }
} // namespace T6
