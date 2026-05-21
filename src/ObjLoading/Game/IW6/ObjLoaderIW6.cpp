#include "ObjLoaderIW6.h"

#include "Asset/GlobalAssetPoolsLoader.h"
#include "Game/IW6/AssetMarkerIW6.h"
#include "Game/IW6/GameIW6.h"
#include "Game/IW6/IW6.h"
#include "Game/IW6/Image/ImageLoaderEmbeddedIW6.h"
#include "Game/IW6/Image/ImageLoaderExternalIW6.h"
#include "Game/IW6/Techset/PixelShaderLoaderIW6.h"
#include "Game/IW6/Techset/VertexShaderLoaderIW6.h"
#include "Game/IW6/XModel/LoaderXModelIW6.h"
#include "Leaderboard/LoaderLeaderboardIW6.h"
#include "Localize/LoaderLocalizeIW6.h"
#include "Material/LoaderMaterialIW6.h"
#include "Menu/LoaderMenuListIW6.h"
#include "ObjLoading.h"
#include "PhysPreset/GdtLoaderPhysPresetIW6.h"
#include "PhysPreset/RawLoaderPhysPresetIW6.h"
#include "RawFile/LoaderRawFileIW6.h"
#include "Script/LoaderScriptFileIW6.h"
#include "Sound/LoaderSoundCurveIW6.h"
#include "StringTable/LoaderStringTableIW6.h"
#include "Weapon/GdtLoaderWeaponIW6.h"
#include "Weapon/LoaderAttachmentIW6.h"
#include "Weapon/RawLoaderWeaponIW6.h"

#include <memory>

using namespace IW6;

void ObjLoader::LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const {}

void ObjLoader::UnloadContainersOfZone(Zone& zone) const {}

namespace
{
    void ConfigureDefaultCreators(AssetCreatorCollection& collection, Zone& zone)
    {
        auto& memory = zone.Memory();

        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetPhysPreset>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetPhysCollMap>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetXAnim>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetXModelSurfs>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetXModel>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetMaterial>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetComputeShader>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetVertexShader>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetHullShader>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetDomainShader>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetPixelShader>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetVertexDecl>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetTechniqueSet>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetImage>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetSound>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetSoundCurve>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetLpfCurve>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetReverbCurve>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetLoadedSound>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetClipMap>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetComWorld>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetGlassWorld>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetPathData>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetVehicleTrack>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetMapEnts>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetFxWorld>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetGfxWorld>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetLightDef>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetFont>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetMenuList>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetMenu>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetAnimationClass>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetLocalize>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetAttachment>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetWeapon>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetFx>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetImpactFx>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetSurfaceFx>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetRawFile>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetScript>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetStringTable>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetLeaderboard>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetStructuredDataDef>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetTracer>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetVehicle>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetAddonMapEnts>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetNetConstStrings>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetReverbPreset>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetLuaFile>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetScriptable>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetColorization>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetColorizationSet>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetToneMapping>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetEquipmentSoundTable>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetVectorField>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetDopplerPreset>>(memory));
        collection.AddDefaultAssetCreator(std::make_unique<DefaultAssetCreator<AssetFxParticleSimAnimation>>(memory));
    }

    void ConfigureGlobalAssetPoolsLoaders(AssetCreatorCollection& collection, Zone& zone)
    {
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPhysPreset>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPhysCollMap>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetXAnim>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetXModelSurfs>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetXModel>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMaterial>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetComputeShader>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetVertexShader>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetHullShader>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetDomainShader>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPixelShader>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetVertexDecl>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetTechniqueSet>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetImage>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetSound>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetSoundCurve>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLpfCurve>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetReverbCurve>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLoadedSound>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetClipMap>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetComWorld>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetGlassWorld>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetPathData>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetVehicleTrack>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMapEnts>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetFxWorld>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetGfxWorld>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLightDef>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetFont>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMenuList>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetMenu>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetAnimationClass>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLocalize>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetAttachment>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetWeapon>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetFx>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetImpactFx>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetSurfaceFx>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetRawFile>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetScript>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetStringTable>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLeaderboard>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetStructuredDataDef>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetTracer>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetVehicle>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetAddonMapEnts>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetNetConstStrings>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetReverbPreset>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetLuaFile>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetScriptable>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetColorization>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetColorizationSet>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetToneMapping>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetEquipmentSoundTable>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetVectorField>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetDopplerPreset>>(zone));
        collection.AddAssetCreator(std::make_unique<GlobalAssetPoolsLoader<AssetFxParticleSimAnimation>>(zone));
    }

    void ConfigureLoaders(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath, IGdtQueryable& gdt)
    {
        auto& memory = zone.Memory();

        collection.AddAssetCreator(phys_preset::CreateRawLoaderIW6(memory, searchPath, zone));
        collection.AddAssetCreator(phys_preset::CreateGdtLoaderIW6(memory, gdt, zone));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderPhysCollMap>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderXAnim>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderXModelSurfs>(memory));
        collection.AddAssetCreator(xmodel::CreateLoaderIW6(memory, searchPath, zone));
        collection.AddAssetCreator(material::CreateLoaderIW6(memory, searchPath));
        // collection.AddAssetCreator(techset::CreateComputeShaderLoaderIW6(memory, searchPath));
        collection.AddAssetCreator(techset::CreateVertexShaderLoaderIW6(memory, searchPath));
        // collection.AddAssetCreator(techset::CreateHullShaderLoaderIW6(memory, searchPath));
        // collection.AddAssetCreator(techset::CreateDomainShaderLoaderIW6(memory, searchPath));
        collection.AddAssetCreator(techset::CreatePixelShaderLoaderIW6(memory, searchPath));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderTechniqueSet>(memory));
        collection.AddAssetCreator(image::CreateLoaderEmbeddedIW6(memory, searchPath));
        collection.AddAssetCreator(image::CreateLoaderExternalIW6(memory, searchPath));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderSound>(memory));
        collection.AddAssetCreator(sound_curve::CreateLoaderIW6(memory, searchPath));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderLoadedSound>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderClipMap>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderComWorld>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderGlassWorld>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderPathData>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderVehicleTrack>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMapEnts>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderFxWorld>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderGfxWorld>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderLightDef>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderFont>(memory));
        collection.AddAssetCreator(menu::CreateMenuListLoaderIW6(memory, searchPath));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderMenu>(memory));
        collection.AddAssetCreator(localize::CreateLoaderIW6(memory, searchPath, zone));
        collection.AddAssetCreator(attachment::CreateLoaderIW6(memory, searchPath));
        collection.AddAssetCreator(weapon::CreateRawLoaderIW6(memory, searchPath, zone));
        collection.AddAssetCreator(weapon::CreateGdtLoaderIW6(memory, searchPath, gdt, zone));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderFx>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderImpactFx>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderSurfaceFx>(memory));
        collection.AddAssetCreator(raw_file::CreateLoaderIW6(memory, searchPath));
        collection.AddAssetCreator(script::CreateLoaderIW6(memory, searchPath));
        collection.AddAssetCreator(string_table::CreateLoaderIW6(memory, searchPath));
        collection.AddAssetCreator(leaderboard::CreateLoaderIW6(memory, searchPath));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderStructuredDataDef>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderTracer>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderVehicle>(memory));
        // collection.AddAssetCreator(std::make_unique<AssetLoaderAddonMapEnts>(memory));
    }
} // namespace

void ObjLoader::ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath, IGdtQueryable& gdt) const
{
    ConfigureDefaultCreators(collection, zone);
    ConfigureLoaders(collection, zone, searchPath, gdt);
    ConfigureGlobalAssetPoolsLoaders(collection, zone);
}
