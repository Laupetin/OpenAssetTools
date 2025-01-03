#include "ObjLoaderT5.h"

#include "Asset/GlobalAssetPoolsLoader.h"
#include "Game/T5/GameT5.h"
#include "Game/T5/T5.h"
#include "Game/T5/XModel/LoaderXModelT5.h"
#include "Localize/LoaderLocalizeT5.h"
#include "ObjLoading.h"
#include "RawFile/LoaderRawFileT5.h"
#include "StringTable/LoaderStringTableT5.h"

#include <memory>

using namespace T5;

void ObjLoader::LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const {}

void ObjLoader::UnloadContainersOfZone(Zone& zone) const {}

namespace
{
    void ConfigureDefaultCreators(AssetCreatorCollection& collection, Zone& zone)
    {
        auto& memory = *zone.GetMemory();

        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetPhysPreset>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetPhysConstraints>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetDestructibleDef>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetXAnim>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetXModel>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetMaterial>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetTechniqueSet>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetImage>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetSoundBank>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetSoundPatch>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetClipMap>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetClipMapPvs>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetComWorld>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetGameWorldSp>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetGameWorldMp>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetMapEnts>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetGfxWorld>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetLightDef>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetFont>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetMenuList>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetMenu>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetLocalize>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetWeapon>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetSoundDriverGlobals>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetFx>>(memory));
        // AssetImpactFx has no name and cannot be default constructed
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetRawFile>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetStringTable>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetPackIndex>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetXGlobals>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetDDL>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetGlasses>>(memory));
        // AssetEmblemSet has no name and cannot be default constructed
    }

    void ConfigureGlobalAssetPoolsLoaders(AssetCreatorCollection& collection, Zone& zone)
    {
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPhysPreset>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPhysConstraints>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetDestructibleDef>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetXAnim>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetXModel>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMaterial>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetTechniqueSet>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetImage>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetSoundBank>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetSoundPatch>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetClipMap>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetClipMapPvs>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetComWorld>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetGameWorldSp>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetGameWorldMp>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMapEnts>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetGfxWorld>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLightDef>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetFont>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMenuList>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMenu>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLocalize>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetWeapon>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetSoundDriverGlobals>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetFx>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetImpactFx>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetRawFile>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetStringTable>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPackIndex>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetXGlobals>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetDDL>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetGlasses>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetEmblemSet>>(zone));
    }

    void ConfigureLoaders(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath)
    {
        auto& memory = *zone.GetMemory();

        // collection.AddAssetCreator(std::make_unique<AssetLoaderPhysPreset>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderPhysConstraints>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderDestructibleDef>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderXAnim>(memory));
        collection.AddAssetCreator(CreateXModelLoader(memory, searchPath, zone));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMaterial>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderTechniqueSet>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderImage>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderSoundBank>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderSoundPatch>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderClipMap>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderClipMapPvs>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderComWorld>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderGameWorldSp>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderGameWorldMp>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMapEnts>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderGfxWorld>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderLightDef>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderFont>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMenuList>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMenu>(memory));
        collection.AddAssetCreator(CreateLocalizeLoader(memory, searchPath, zone));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderWeapon>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderSoundDriverGlobals>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderFx>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderImpactFx>(memory));
        collection.AddAssetCreator(CreateRawFileLoader(memory, searchPath));
        collection.AddAssetCreator(CreateStringTableLoader(memory, searchPath));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderPackIndex>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderXGlobals>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderDDL>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderGlasses>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderEmblemSet>(memory));
    }
} // namespace

void ObjLoader::ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath, IGdtQueryable& gdt) const
{
    ConfigureDefaultCreators(collection, zone);
    ConfigureLoaders(collection, zone, searchPath);
    ConfigureGlobalAssetPoolsLoaders(collection, zone);
}
