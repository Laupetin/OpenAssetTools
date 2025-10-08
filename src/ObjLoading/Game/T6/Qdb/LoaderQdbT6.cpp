#include "LoaderQdbT6.h"

#include "Game/T6/T6.h"

#include <cstring>

using namespace T6;

namespace
{
    class QdbLoader final : public AssetCreator<AssetQdb>
    {
    public:
        QdbLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(assetName);
            if (!file.IsOpen())
                return AssetCreationResult::Failure();

            auto* qdb = m_memory.Alloc<Qdb>();
            qdb->name = m_memory.Dup(assetName.c_str());
            qdb->len = static_cast<int>(file.m_length);

            auto* fileBuffer = m_memory.Alloc<char>(static_cast<size_t>(file.m_length + 1));
            file.m_stream->read(fileBuffer, file.m_length);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();
            fileBuffer[qdb->len] = '\0';

            qdb->buffer = static_cast<char16*>(fileBuffer);

            return AssetCreationResult::Success(context.AddAsset<AssetQdb>(assetName, qdb));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace qdb
{
    std::unique_ptr<AssetCreator<AssetQdb>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<QdbLoader>(memory, searchPath);
    }
} // namespace qdb
