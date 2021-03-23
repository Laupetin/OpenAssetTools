#include "AssetLoaderSlug.h"

#include <cstring>

#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"

using namespace T6;

void* AssetLoaderSlug::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* slug = memory->Create<Slug>();
    memset(slug, 0, sizeof(Slug));
    slug->name = memory->Dup(assetName.c_str());
    return slug;
}

bool AssetLoaderSlug::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderSlug::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    auto* slug = memory->Create<Slug>();
    slug->name = memory->Dup(assetName.c_str());
    slug->len = static_cast<int>(file.m_length);

    auto* fileBuffer = static_cast<char*>(memory->Alloc(static_cast<size_t>(file.m_length + 1)));
    file.m_stream->read(fileBuffer, file.m_length);
    if (file.m_stream->gcount() != file.m_length)
        return false;
    fileBuffer[slug->len] = '\0';

    slug->buffer = static_cast<char16*>(fileBuffer);
    manager->AddAsset(ASSET_TYPE_SLUG, assetName, slug);

    return true;
}
