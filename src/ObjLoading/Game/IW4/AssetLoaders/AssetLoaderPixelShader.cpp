#include "AssetLoaderPixelShader.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace IW4;

void* AssetLoaderPixelShader::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* pixelShader = memory->Create<MaterialPixelShader>();
    memset(pixelShader, 0, sizeof(MaterialPixelShader));
    pixelShader->name = memory->Dup(assetName.c_str());
    return pixelShader;
}

bool AssetLoaderPixelShader::CanLoadFromRaw() const
{
    return true;
}

std::string AssetLoaderPixelShader::GetFileNameForAsset(const std::string& assetName)
{
    std::ostringstream ss;
    ss << "shader_bin/ps_" << assetName << ".cso";
    return ss.str();
}

bool AssetLoaderPixelShader::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = GetFileNameForAsset(assetName);
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    if (file.m_length % sizeof(uint32_t) != 0)
    {
        std::cerr << "Invalid pixel shader \"" << assetName << "\": Size must be dividable by " << sizeof(uint32_t) << "\n";
        return false;
    }

    auto* pixelShader = memory->Create<MaterialPixelShader>();
    pixelShader->name = memory->Dup(assetName.c_str());
    pixelShader->prog.loadDef.programSize = static_cast<uint16_t>(static_cast<size_t>(file.m_length) / sizeof(uint32_t));
    pixelShader->prog.loadDef.loadForRenderer = 0;
    pixelShader->prog.ps = nullptr;

    auto* fileBuffer = static_cast<char*>(memory->Alloc(pixelShader->prog.loadDef.programSize * sizeof(uint32_t)));
    file.m_stream->read(fileBuffer, static_cast<std::streamsize>(pixelShader->prog.loadDef.programSize) * sizeof(uint32_t));
    if (file.m_stream->gcount() != file.m_length)
        return false;

    pixelShader->prog.loadDef.program = reinterpret_cast<uint32_t*>(fileBuffer);
    manager->AddAsset(ASSET_TYPE_PIXELSHADER, assetName, pixelShader);

    return true;
}
