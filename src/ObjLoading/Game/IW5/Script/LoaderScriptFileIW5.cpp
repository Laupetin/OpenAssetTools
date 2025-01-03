#include "LoaderScriptFileIW5.h"

#include "Game/IW5/IW5.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <filesystem>
#include <format>
#include <iostream>

using namespace IW5;

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
            // See https://github.com/xensik/gsc-tool#file-format for an in-depth explanation about the .gscbin format
            const auto file = m_search_path.Open(std::format("{}.gscbin", assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            const auto fileBuffer = std::make_unique<char[]>(static_cast<size_t>(file.m_length));
            file.m_stream->read(fileBuffer.get(), file.m_length);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();

            auto* scriptFile = m_memory.Alloc<ScriptFile>();
            scriptFile->name = m_memory.Dup(assetName.c_str());

            // Retrieve data from the buffer
            size_t offset = 0;

            // Read past the name pointer, we will use the one from assetName
            offset += strlen(fileBuffer.get()) + 1;

            std::memcpy(&scriptFile->compressedLen, fileBuffer.get() + offset, sizeof(scriptFile->compressedLen));
            offset += sizeof(scriptFile->compressedLen);

            std::memcpy(&scriptFile->len, fileBuffer.get() + offset, sizeof(scriptFile->len));
            offset += sizeof(scriptFile->len);

            std::memcpy(&scriptFile->bytecodeLen, fileBuffer.get() + offset, sizeof(scriptFile->bytecodeLen));
            offset += sizeof(scriptFile->bytecodeLen);

            if (scriptFile->compressedLen <= 0 || scriptFile->bytecodeLen <= 0)
            {
                std::cerr << std::format("Error: Invalid length of the buffers in {} specified\n", assetName);
                return AssetCreationResult::Failure();
            }

            if (offset + (scriptFile->compressedLen + scriptFile->bytecodeLen) > file.m_length)
            {
                std::cerr << std::format("Error: Specified length in {} GSC BIN structure exceeds the actual file size\n", assetName);
                return AssetCreationResult::Failure();
            }

            scriptFile->buffer = m_memory.Alloc<char>(scriptFile->compressedLen);
            std::memcpy(const_cast<char*>(scriptFile->buffer), fileBuffer.get() + offset, scriptFile->compressedLen);
            offset += scriptFile->compressedLen;

            scriptFile->bytecode = m_memory.Alloc<unsigned char>(scriptFile->bytecodeLen);
            std::memcpy(scriptFile->bytecode, fileBuffer.get() + offset, scriptFile->bytecodeLen);

            return AssetCreationResult::Success(context.AddAsset<AssetScript>(assetName, scriptFile));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW5
{
    std::unique_ptr<AssetCreator<AssetScript>> CreateScriptLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<ScriptLoader>(memory, searchPath);
    }
} // namespace IW5
