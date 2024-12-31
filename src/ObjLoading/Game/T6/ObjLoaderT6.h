#pragma once

#include "AssetLoading/IAssetLoader.h"
#include "Game/T6/T6.h"
#include "IObjLoader.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "SearchPath/ISearchPath.h"

#include <memory>
#include <set>
#include <stack>
#include <string>

namespace T6
{
    class ObjLoader final : public IObjLoader
    {
    public:
        void LoadReferencedContainersForZone(ISearchPath& searchPath, Zone& zone) const override;
        void UnloadContainersOfZone(Zone& zone) const override;

        void ConfigureCreatorCollection(AssetCreatorCollection& collection, Zone& zone, ISearchPath& searchPath, IGdtQueryable& gdt) const override;

    private:
        static bool VerifySoundBankChecksum(const SoundBank& soundBank, const SndRuntimeAssetBank& sndRuntimeAssetBank);
        static SoundBank* LoadSoundBankForZone(ISearchPath& searchPath, const std::string& soundBankFileName, Zone& zone);
        static void LoadSoundBankFromLinkedInfo(ISearchPath& searchPath,
                                                const std::string& soundBankFileName,
                                                const SndRuntimeAssetBank& sndBankLinkedInfo,
                                                Zone& zone,
                                                std::set<std::string>& loadedBanksForZone,
                                                std::stack<std::string>& dependenciesToLoad);
        static void LoadSoundBanksFromAsset(ISearchPath& searchPath, const SndBank& sndBank, Zone& zone, std::set<std::string>& loadedBanksForZone);

        static void LoadIPakForZone(ISearchPath& searchPath, const std::string& ipakName, Zone& zone);
        static void LoadCommonIPaks(ISearchPath& searchPath, Zone& zone);

        static bool IsMpZone(const Zone& zone);
        static bool IsZmZone(const Zone& zone);
    };
} // namespace T6
