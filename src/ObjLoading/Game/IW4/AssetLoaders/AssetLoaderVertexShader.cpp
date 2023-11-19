#include "AssetLoaderVertexShader.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace IW4;

void* AssetLoaderVertexShader::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vertexShader = memory->Create<MaterialVertexShader>();
    memset(vertexShader, 0, sizeof(MaterialVertexShader));
    vertexShader->name = memory->Dup(assetName.c_str());
    return vertexShader;
}

bool AssetLoaderVertexShader::CanLoadFromRaw() const
{
    return true;
}

std::string AssetLoaderVertexShader::GetFileNameForAsset(const std::string& assetName)
{
    std::ostringstream ss;
    ss << "shader_bin/vs_" << assetName << ".cso";
    return ss.str();
}

bool AssetLoaderVertexShader::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = GetFileNameForAsset(assetName);
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    if (file.m_length % sizeof(uint32_t) != 0)
    {
        std::cerr << "Invalid vertex shader \"" << assetName << "\": Size must be dividable by " << sizeof(uint32_t) << "\n";
        return false;
    }

    auto* vertexShader = memory->Create<MaterialVertexShader>();
    vertexShader->name = memory->Dup(assetName.c_str());
    vertexShader->prog.loadDef.programSize = static_cast<uint16_t>(static_cast<size_t>(file.m_length) / sizeof(uint32_t));
    vertexShader->prog.loadDef.loadForRenderer = 0;
    vertexShader->prog.vs = nullptr;

    auto* fileBuffer = static_cast<char*>(memory->Alloc(vertexShader->prog.loadDef.programSize * sizeof(uint32_t)));
    file.m_stream->read(fileBuffer, static_cast<std::streamsize>(vertexShader->prog.loadDef.programSize) * sizeof(uint32_t));
    if (file.m_stream->gcount() != file.m_length)
        return false;

    vertexShader->prog.loadDef.program = reinterpret_cast<uint32_t*>(fileBuffer);
    manager->AddAsset(ASSET_TYPE_VERTEXSHADER, assetName, vertexShader);

    return true;
}
