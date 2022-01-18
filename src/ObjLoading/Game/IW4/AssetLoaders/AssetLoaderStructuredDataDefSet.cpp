#include "AssetLoaderStructuredDataDefSet.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Parsing/StructuredDataDef/StructuredDataDefReader.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderStructuredDataDefSet::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* structuredDataDefSet = memory->Create<StructuredDataDefSet>();
    memset(structuredDataDefSet, 0, sizeof(StructuredDataDefSet));
    structuredDataDefSet->name = memory->Dup(assetName.c_str());
    return structuredDataDefSet;
}

bool AssetLoaderStructuredDataDefSet::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderStructuredDataDefSet::LoadFromRaw(const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    auto* structuredDataDefSet = memory->Create<StructuredDataDefSet>();
    structuredDataDefSet->name = memory->Dup(assetName.c_str());

    StructuredDataDefReader reader(*file.m_stream, assetName, [searchPath](const std::string& filename, const std::string& sourceFile) -> std::unique_ptr<std::istream>
    {
        auto foundFileToInclude = searchPath->Open(filename);
        if (!foundFileToInclude.IsOpen() || !foundFileToInclude.m_stream)
            return nullptr;

        return std::move(foundFileToInclude.m_stream);
    });

    const auto defs = reader.ReadStructureDataDefs();

    // TODO Convert defs

    manager->AddAsset(ASSET_TYPE_STRUCTURED_DATA_DEF, assetName, structuredDataDefSet);

    return true;
}
