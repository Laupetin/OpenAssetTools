#include "LoaderScriptT6.h"

#include "Game/T6/T6.h"

#include <cstring>

using namespace T6;

namespace
{
    class ScriptLoader final : public AssetCreator<AssetScript>
    {
    public:
        ScriptLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(assetName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* scriptParseTree = m_memory.Alloc<ScriptParseTree>();
            scriptParseTree->name = m_memory.Dup(assetName.c_str());
            scriptParseTree->len = static_cast<int>(file.m_length);

            auto* fileBuffer = m_memory.Alloc<char>(static_cast<size_t>(file.m_length + 1));
            file.m_stream->read(fileBuffer, file.m_length);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();
            fileBuffer[scriptParseTree->len] = '\0';

            scriptParseTree->buffer = static_cast<char16*>(fileBuffer);

            return AssetCreationResult::Success(context.AddAsset<AssetScript>(assetName, scriptParseTree));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace T6::script
{
    std::unique_ptr<AssetCreator<AssetScript>> CreateLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<ScriptLoader>(memory, searchPath);
    }
} // namespace T6::script
