#include "LoaderScriptT6.h"

#include "Game/T6/T6.h"

#include <cstring>

#include "ScriptCompileT6.h"

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

        const unsigned char T6GSCMagic[8] = {0x80, 0x47, 0x53, 0x43, 0x0D,  0x0A, 0x00, 0x06 };

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(assetName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            

            auto* fileBuffer = m_memory.Alloc<char>(static_cast<size_t>(file.m_length + 1));
            file.m_stream->read(fileBuffer, file.m_length);
            if (file.m_stream->gcount() != file.m_length || file.m_length < 0x10)
                return AssetCreationResult::Failure();
            fileBuffer[file.m_length] = '\0';

            auto* scriptParseTree = m_memory.Alloc<ScriptParseTree>();
            scriptParseTree->name = m_memory.Dup(assetName.c_str());

            if (memcmp(fileBuffer, T6GSCMagic, 8) == 0)
            {
                scriptParseTree->len = static_cast<int>(file.m_length);
                scriptParseTree->buffer = static_cast<char16*>(fileBuffer);
            }
            else
            {
                size_t compiledSize;
                char* result = compileScriptT6(assetName, fileBuffer, static_cast<size_t>(file.m_length), &compiledSize);
                if (result == NULL)
                    return AssetCreationResult::Failure();

                scriptParseTree->buffer = result;
                scriptParseTree->len = static_cast<int>(compiledSize);
            }

            return AssetCreationResult::Success(context.AddAsset<AssetScript>(assetName, scriptParseTree));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace T6
{
    std::unique_ptr<AssetCreator<AssetScript>> CreateScriptLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<ScriptLoader>(memory, searchPath);
    }
} // namespace T6
