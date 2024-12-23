#include "ObjLoaderIW4.h"

#include "Asset/GlobalAssetPoolsLoader.h"
#include "Game/IW4/GameIW4.h"
#include "Game/IW4/IW4.h"
#include "Localize/AssetLoaderLocalizeIW4.h"
#include "ObjLoading.h"

#include <memory>

using namespace IW4;

void ObjLoader::LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const {}

void ObjLoader::UnloadContainersOfZone(Zone& zone) const {}

namespace
{
    void ConfigureDefaultCreators(AssetCreatorCollection& collection, Zone& zone)
    {
        auto& memory = *zone.GetMemory();
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorPhysPreset>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorPhysCollMap>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorXAnim>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorXModelSurfs>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorXModel>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorMaterial>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorPixelShader>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorVertexShader>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorVertexDecl>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorTechniqueSet>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorImage>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorSound>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorSoundCurve>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorLoadedSound>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorClipMapSp>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorClipMapMp>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorComWorld>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorGameWorldSp>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorGameWorldMp>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorMapEnts>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorFxWorld>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorGfxWorld>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorLightDef>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorFont>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorMenuList>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorMenu>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorLocalize>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorWeapon>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorFx>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorImpactFx>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorRawFile>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorStringTable>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorLeaderboard>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorStructuredDataDef>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorTracer>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorVehicle>(memory));
        // collection.AddDefaultAssetCreator(std::make_unique<DefaultCreatorAddonMapEnts>(memory));
    }

    void ConfigureGlobalAssetPoolsLoaders(AssetCreatorCollection& collection, Zone& zone)
    {
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPhysPreset>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPhysCollMap>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetXAnim>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetXModelSurfs>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetXModel>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMaterial>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPixelShader>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetVertexShader>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetVertexDecl>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetTechniqueSet>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetImage>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetSound>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetSoundCurve>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLoadedSound>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetClipMapSp>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetClipMapMp>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetComWorld>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetGameWorldSp>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetGameWorldMp>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMapEnts>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetFxWorld>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetGfxWorld>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLightDef>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetFont>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMenuList>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMenu>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLocalize>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetWeapon>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetFx>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetImpactFx>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetRawFile>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetStringTable>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLeaderboard>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetStructuredDataDef>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetTracer>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetVehicle>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetAddonMapEnts>>(zone));
    }

    void ConfigureDefaultCreators(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath)
    {
        auto& memory = *zone.GetMemory();

        // collection.AddAssetCreator(std::make_unique<AssetLoaderPhysPreset>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderPhysCollMap>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderXAnim>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderXModelSurfs>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderXModel>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMaterial>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderPixelShader>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderVertexShader>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderVertexDecl>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderTechniqueSet>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderImage>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderSound>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderSoundCurve>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderLoadedSound>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderClipMapSp>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderClipMapMp>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderComWorld>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderGameWorldSp>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderGameWorldMp>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMapEnts>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderFxWorld>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderGfxWorld>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderLightDef>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderFont>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMenuList>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMenu>(memory));
        collection.AddAssetCreator(std::make_unique<AssetLoaderLocalize>(memory, searchPath, zone));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderWeapon>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderFx>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderImpactFx>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderRawFile>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderStringTable>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderLeaderboard>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderStructuredDataDef>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderTracer>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderVehicle>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderAddonMapEnts>(memory));
    }
} // namespace

void ObjLoader::ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath) const
{
    ConfigureDefaultCreators(collection, zone, searchPath);
    ConfigureGlobalAssetPoolsLoaders(collection, zone);
}
