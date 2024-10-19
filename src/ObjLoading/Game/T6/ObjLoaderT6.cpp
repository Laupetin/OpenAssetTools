#include "ObjLoaderT6.h"

#include "AssetLoaders/AssetLoaderAddonMapEnts.h"
#include "AssetLoaders/AssetLoaderClipMap.h"
#include "AssetLoaders/AssetLoaderComWorld.h"
#include "AssetLoaders/AssetLoaderDDL.h"
#include "AssetLoaders/AssetLoaderDestructibleDef.h"
#include "AssetLoaders/AssetLoaderEmblemSet.h"
#include "AssetLoaders/AssetLoaderFont.h"
#include "AssetLoaders/AssetLoaderFontIcon.h"
#include "AssetLoaders/AssetLoaderFootstepFxTable.h"
#include "AssetLoaders/AssetLoaderFootstepTable.h"
#include "AssetLoaders/AssetLoaderFx.h"
#include "AssetLoaders/AssetLoaderGameWorldMp.h"
#include "AssetLoaders/AssetLoaderGameWorldSp.h"
#include "AssetLoaders/AssetLoaderGfxImage.h"
#include "AssetLoaders/AssetLoaderGfxLightDef.h"
#include "AssetLoaders/AssetLoaderGfxWorld.h"
#include "AssetLoaders/AssetLoaderGlasses.h"
#include "AssetLoaders/AssetLoaderImpactFx.h"
#include "AssetLoaders/AssetLoaderLeaderboard.h"
#include "AssetLoaders/AssetLoaderLocalizeEntry.h"
#include "AssetLoaders/AssetLoaderMapEnts.h"
#include "AssetLoaders/AssetLoaderMaterial.h"
#include "AssetLoaders/AssetLoaderMemoryBlock.h"
#include "AssetLoaders/AssetLoaderMenu.h"
#include "AssetLoaders/AssetLoaderMenuList.h"
#include "AssetLoaders/AssetLoaderPhysConstraints.h"
#include "AssetLoaders/AssetLoaderPhysPreset.h"
#include "AssetLoaders/AssetLoaderQdb.h"
#include "AssetLoaders/AssetLoaderRawFile.h"
#include "AssetLoaders/AssetLoaderScriptParseTree.h"
#include "AssetLoaders/AssetLoaderSkinnedVerts.h"
#include "AssetLoaders/AssetLoaderSlug.h"
#include "AssetLoaders/AssetLoaderSoundBank.h"
#include "AssetLoaders/AssetLoaderSoundDriverGlobals.h"
#include "AssetLoaders/AssetLoaderSoundPatch.h"
#include "AssetLoaders/AssetLoaderStringTable.h"
#include "AssetLoaders/AssetLoaderTechniqueSet.h"
#include "AssetLoaders/AssetLoaderTracer.h"
#include "AssetLoaders/AssetLoaderVehicle.h"
#include "AssetLoaders/AssetLoaderWeapon.h"
#include "AssetLoaders/AssetLoaderWeaponAttachment.h"
#include "AssetLoaders/AssetLoaderWeaponAttachmentUnique.h"
#include "AssetLoaders/AssetLoaderWeaponCamo.h"
#include "AssetLoaders/AssetLoaderXAnim.h"
#include "AssetLoaders/AssetLoaderXGlobals.h"
#include "AssetLoaders/AssetLoaderXModel.h"
#include "AssetLoaders/AssetLoaderZBarrier.h"
#include "AssetLoading/AssetLoadingManager.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/GameT6.h"
#include "Image/Dx12TextureLoader.h"
#include "Image/IwiLoader.h"
#include "Image/IwiTypes.h"
#include "Image/Texture.h"
#include "ObjContainer/IPak/IPak.h"
#include "ObjLoading.h"

#include <format>

namespace T6
{
    constexpr auto IPAK_READ_HASH = Common::Com_HashKey("ipak_read", 64);
    constexpr auto GLOBAL_HASH = Common::Com_HashKey("GLOBAL", 64);

    ObjLoader::ObjLoader()
    {
#define REGISTER_ASSET_LOADER(t)                                                                                                                               \
    {                                                                                                                                                          \
        auto l = std::make_unique<t>();                                                                                                                        \
        m_asset_loaders_by_type[l->GetHandlingAssetType()] = std::move(l);                                                                                     \
    }

        REGISTER_ASSET_LOADER(AssetLoaderPhysPreset)
        REGISTER_ASSET_LOADER(AssetLoaderPhysConstraints)
        REGISTER_ASSET_LOADER(AssetLoaderDestructibleDef)
        REGISTER_ASSET_LOADER(AssetLoaderXAnim)
        REGISTER_ASSET_LOADER(AssetLoaderXModel)
        REGISTER_ASSET_LOADER(AssetLoaderMaterial)
        REGISTER_ASSET_LOADER(AssetLoaderTechniqueSet)
        REGISTER_ASSET_LOADER(AssetLoaderGfxImage)
        REGISTER_ASSET_LOADER(AssetLoaderSoundBank)
        REGISTER_ASSET_LOADER(AssetLoaderSoundPatch)
        REGISTER_ASSET_LOADER(AssetLoaderClipMap)
        REGISTER_ASSET_LOADER(AssetLoaderClipMapPvs)
        REGISTER_ASSET_LOADER(AssetLoaderComWorld)
        REGISTER_ASSET_LOADER(AssetLoaderGameWorldSp)
        REGISTER_ASSET_LOADER(AssetLoaderGameWorldMp)
        REGISTER_ASSET_LOADER(AssetLoaderMapEnts)
        REGISTER_ASSET_LOADER(AssetLoaderGfxWorld)
        REGISTER_ASSET_LOADER(AssetLoaderGfxLightDef)
        REGISTER_ASSET_LOADER(AssetLoaderFont)
        REGISTER_ASSET_LOADER(AssetLoaderFontIcon)
        REGISTER_ASSET_LOADER(AssetLoaderMenuList)
        REGISTER_ASSET_LOADER(AssetLoaderMenu)
        REGISTER_ASSET_LOADER(AssetLoaderLocalizeEntry)
        REGISTER_ASSET_LOADER(AssetLoaderWeapon)
        REGISTER_ASSET_LOADER(AssetLoaderWeaponAttachment)
        REGISTER_ASSET_LOADER(AssetLoaderWeaponAttachmentUnique)
        REGISTER_ASSET_LOADER(AssetLoaderWeaponCamo)
        REGISTER_ASSET_LOADER(AssetLoaderSoundDriverGlobals)
        REGISTER_ASSET_LOADER(AssetLoaderFx)
        REGISTER_ASSET_LOADER(AssetLoaderImpactFx)
        REGISTER_ASSET_LOADER(AssetLoaderRawFile)
        REGISTER_ASSET_LOADER(AssetLoaderStringTable)
        REGISTER_ASSET_LOADER(AssetLoaderLeaderboard)
        REGISTER_ASSET_LOADER(AssetLoaderXGlobals)
        REGISTER_ASSET_LOADER(AssetLoaderDDL)
        REGISTER_ASSET_LOADER(AssetLoaderGlasses)
        REGISTER_ASSET_LOADER(AssetLoaderEmblemSet)
        REGISTER_ASSET_LOADER(AssetLoaderScriptParseTree)
        REGISTER_ASSET_LOADER(AssetLoaderVehicle)
        REGISTER_ASSET_LOADER(AssetLoaderMemoryBlock)
        REGISTER_ASSET_LOADER(AssetLoaderAddonMapEnts)
        REGISTER_ASSET_LOADER(AssetLoaderTracer)
        REGISTER_ASSET_LOADER(AssetLoaderSkinnedVerts)
        REGISTER_ASSET_LOADER(AssetLoaderQdb)
        REGISTER_ASSET_LOADER(AssetLoaderSlug)
        REGISTER_ASSET_LOADER(AssetLoaderFootstepTable)
        REGISTER_ASSET_LOADER(AssetLoaderFootstepFxTable)
        REGISTER_ASSET_LOADER(AssetLoaderZBarrier)

#undef REGISTER_ASSET_LOADER
    }

    bool ObjLoader::VerifySoundBankChecksum(const SoundBank& soundBank, const SndRuntimeAssetBank& sndRuntimeAssetBank)
    {
        SoundAssetBankChecksum checksum{};
        static_assert(sizeof(SoundAssetBankChecksum::checksumBytes) == sizeof(SndRuntimeAssetBank::linkTimeChecksum));
        for (auto i = 0u; i < sizeof(SoundAssetBankChecksum::checksumBytes); i++)
            checksum.checksumBytes[i] = sndRuntimeAssetBank.linkTimeChecksum[i];

        return soundBank.VerifyChecksum(checksum);
    }

    SoundBank* ObjLoader::LoadSoundBankForZone(ISearchPath& searchPath, const std::string& soundBankFileName, Zone& zone)
    {
        if (ObjLoading::Configuration.Verbose)
            std::cout << std::format("Trying to load sound bank '{}' for zone '{}'\n", soundBankFileName, zone.m_name);

        auto* existingSoundBank = SoundBank::Repository.GetContainerByName(soundBankFileName);
        if (existingSoundBank != nullptr)
        {
            if (ObjLoading::Configuration.Verbose)
                std::cout << std::format("Referencing loaded sound bank '{}'.\n", soundBankFileName);

            SoundBank::Repository.AddContainerReference(existingSoundBank, &zone);
            return existingSoundBank;
        }

        auto file = searchPath.Open(soundBankFileName);
        if (file.IsOpen())
        {
            auto sndBank = std::make_unique<SoundBank>(soundBankFileName, std::move(file.m_stream), file.m_length);
            auto* sndBankPtr = sndBank.get();

            if (!sndBank->Initialize())
            {
                std::cerr << std::format("Failed to load sound bank '{}'\n", soundBankFileName);
                return nullptr;
            }

            SoundBank::Repository.AddContainer(std::move(sndBank), &zone);

            if (ObjLoading::Configuration.Verbose)
                std::cout << std::format("Found and loaded sound bank '{}'\n", soundBankFileName);

            return sndBankPtr;
        }

        std::cerr << std::format("Failed to load sound bank '{}'\n", soundBankFileName);
        return nullptr;
    }

    void ObjLoader::LoadSoundBankFromLinkedInfo(ISearchPath& searchPath,
                                                const std::string& soundBankFileName,
                                                const SndRuntimeAssetBank& sndBankLinkedInfo,
                                                Zone& zone,
                                                std::set<std::string>& loadedBanksForZone,
                                                std::stack<std::string>& dependenciesToLoad)
    {
        if (loadedBanksForZone.find(soundBankFileName) == loadedBanksForZone.end())
        {
            auto* soundBank = LoadSoundBankForZone(searchPath, soundBankFileName, zone);

            if (soundBank)
            {
                if (!VerifySoundBankChecksum(*soundBank, sndBankLinkedInfo))
                    std::cout << std::format("Checksum of sound bank does not match link time checksum for '{}'\n", soundBankFileName);

                loadedBanksForZone.emplace(soundBankFileName);

                for (const auto& dependency : soundBank->GetDependencies())
                {
                    dependenciesToLoad.emplace(dependency);
                }
            }
        }
    }

    void ObjLoader::LoadSoundBanksFromAsset(ISearchPath& searchPath, const SndBank& sndBank, Zone& zone, std::set<std::string>& loadedBanksForZone)
    {
        std::stack<std::string> dependenciesToLoad;

        if (sndBank.streamAssetBank.zone)
        {
            const auto soundBankFileName = SoundBank::GetFileNameForDefinition(true, sndBank.streamAssetBank.zone, sndBank.streamAssetBank.language);
            LoadSoundBankFromLinkedInfo(searchPath, soundBankFileName, sndBank.streamAssetBank, zone, loadedBanksForZone, dependenciesToLoad);
        }

        if (sndBank.runtimeAssetLoad && sndBank.loadAssetBank.zone)
        {
            const auto soundBankFileName = SoundBank::GetFileNameForDefinition(false, sndBank.loadAssetBank.zone, sndBank.loadAssetBank.language);
            LoadSoundBankFromLinkedInfo(searchPath, soundBankFileName, sndBank.loadAssetBank, zone, loadedBanksForZone, dependenciesToLoad);
        }

        while (!dependenciesToLoad.empty())
        {
            auto dependencyFileName = dependenciesToLoad.top();
            dependenciesToLoad.pop();

            if (loadedBanksForZone.find(dependencyFileName) == loadedBanksForZone.end())
            {
                auto* soundBank = LoadSoundBankForZone(searchPath, dependencyFileName, zone);

                if (soundBank)
                {
                    loadedBanksForZone.emplace(dependencyFileName);

                    for (const auto& dependency : soundBank->GetDependencies())
                        dependenciesToLoad.emplace(dependency);
                }
            }
        }
    }

    void ObjLoader::LoadIPakForZone(ISearchPath& searchPath, const std::string& ipakName, Zone& zone)
    {
        if (ObjLoading::Configuration.Verbose)
            std::cout << std::format("Trying to load ipak '{}' for zone '{}'\n", ipakName, zone.m_name);

        auto* existingIPak = IPak::Repository.GetContainerByName(ipakName);
        if (existingIPak != nullptr)
        {
            if (ObjLoading::Configuration.Verbose)
                std::cout << std::format("Referencing loaded ipak '{}'.\n", ipakName);

            IPak::Repository.AddContainerReference(existingIPak, &zone);
            return;
        }

        const auto ipakFilename = std::format("{}.ipak", ipakName);

        auto file = searchPath.Open(ipakFilename);
        if (file.IsOpen())
        {
            auto ipak = std::make_unique<IPak>(ipakFilename, std::move(file.m_stream));

            if (ipak->Initialize())
            {
                IPak::Repository.AddContainer(std::move(ipak), &zone);

                if (ObjLoading::Configuration.Verbose)
                    std::cout << std::format("Found and loaded ipak '{}'.\n", ipakFilename);
            }
            else
            {
                std::cerr << std::format("Failed to load ipak '{}'!\n", ipakFilename);
            }
        }
    }

    bool ObjLoader::IsMpZone(const Zone& zone)
    {
        return zone.m_name.compare(0, 3, "mp_") == 0 || zone.m_name.compare(zone.m_name.length() - 3, 3, "_mp") == 0;
    }

    bool ObjLoader::IsZmZone(const Zone& zone)
    {
        return zone.m_name.compare(0, 3, "zm_") == 0 || zone.m_name.compare(zone.m_name.length() - 3, 3, "_zm") == 0;
    }

    void ObjLoader::LoadCommonIPaks(ISearchPath& searchPath, Zone& zone)
    {
        if (ObjLoading::Configuration.Verbose)
            std::cout << std::format("Loading common ipaks for zone \"{}\"\n", zone.m_name);

        LoadIPakForZone(searchPath, "base", zone);
        const auto languagePrefixes = g_GameT6.GetLanguagePrefixes();
        for (const auto& languagePrefix : languagePrefixes)
            LoadIPakForZone(searchPath, std::format("{}base", languagePrefix.m_prefix), zone);

        if (IsMpZone(zone))
        {
            if (ObjLoading::Configuration.Verbose)
                std::cout << std::format("Loading multiplayer ipaks for zone \"{}\"\n", zone.m_name);

            LoadIPakForZone(searchPath, "mp", zone);
            LoadIPakForZone(searchPath, "so", zone);
        }
        else if (IsZmZone(zone))
        {
            if (ObjLoading::Configuration.Verbose)
                std::cout << std::format("Loading zombie ipak for zone \"{}\"\n", zone.m_name);

            LoadIPakForZone(searchPath, "zm", zone);
        }
        else
        {
            if (ObjLoading::Configuration.Verbose)
                std::cout << std::format("Loading singleplayer ipak for zone \"{}\"\n", zone.m_name);

            LoadIPakForZone(searchPath, "sp", zone);
        }
    }

    void ObjLoader::LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const
    {
        const auto* assetPoolT6 = dynamic_cast<GameAssetPoolT6*>(zone.m_pools.get());
        const auto zoneNameHash = Common::Com_HashKey(zone.m_name.c_str(), 64);

        LoadCommonIPaks(searchPath, zone);

        if (assetPoolT6->m_key_value_pairs != nullptr)
        {
            for (auto* keyValuePairsEntry : *assetPoolT6->m_key_value_pairs)
            {
                const auto* keyValuePairs = keyValuePairsEntry->Asset();
                for (auto variableIndex = 0; variableIndex < keyValuePairs->numVariables; variableIndex++)
                {
                    auto* variable = &keyValuePairs->keyValuePairs[variableIndex];

                    if (variable->namespaceHash == zoneNameHash && variable->keyHash == IPAK_READ_HASH)
                    {
                        LoadIPakForZone(searchPath, variable->value, zone);
                    }
                }
            }
        }

        if (assetPoolT6->m_sound_bank != nullptr)
        {
            std::set<std::string> loadedSoundBanksForZone;

            for (auto* sndBankAssetInfo : *assetPoolT6->m_sound_bank)
            {
                LoadSoundBanksFromAsset(searchPath, *sndBankAssetInfo->Asset(), zone, loadedSoundBanksForZone);
            }
        }
    }

    void ObjLoader::UnloadContainersOfZone(Zone& zone) const
    {
        IPak::Repository.RemoveContainerReferences(&zone);
    }

    bool ObjLoader::LoadAssetForZone(AssetLoadingContext& context, const asset_type_t assetType, const std::string& assetName) const
    {
        AssetLoadingManager assetLoadingManager(m_asset_loaders_by_type, context);
        return assetLoadingManager.LoadAssetFromLoader(assetType, assetName);
    }

    void ObjLoader::FinalizeAssetsForZone(AssetLoadingContext& context) const
    {
        for (const auto& [type, loader] : m_asset_loaders_by_type)
            loader->FinalizeAssetsForZone(context);
    }
} // namespace T6
