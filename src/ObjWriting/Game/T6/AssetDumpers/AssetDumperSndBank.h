#pragma once

#include <memory>
#include <ostream>
#include <string>

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "ObjContainer/SoundBank/SoundBankTypes.h"

namespace T6
{
    class AssetDumperSndBank final : public IAssetDumper<SndBank>
    {
        static std::string GetExtensionForFormat(snd_asset_format format);
        std::unique_ptr<std::ostream> OpenAssetOutputFile(AssetDumpingContext& context, const std::string& outputFileName, const SoundAssetBankEntry& entry) const;
        std::unique_ptr<std::ostream> OpenAliasOutputFile(AssetDumpingContext& context, SndBank* sndBank) const;
        void DumpSndBankAliases(AssetDumpingContext& context, SndBank* sndBank, std::unordered_map<unsigned int, std::string>& aliasFiles);
        void DumpSndBank(AssetDumpingContext& context, const XAssetInfo<SndBank>* sndBankInfo);

    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<SndBank>* pool) override;
    };
}
