#include "AssetDumperSndBank.h"

#include "Game/T6/Sound/AliasDumperCsv.h"
#include "Game/T6/Sound/SoundFileDumper.h"

using namespace T6;

namespace T6::sound
{
    void DumpSndBank(const AssetDumpingContext& context, const XAssetInfo<SndBank>* sndBankInfo)
    {
        const auto* sndBank = sndBankInfo->Asset();

        const AliasDumperCsv aliasDumper(context);
        aliasDumper.DumpSndBankAliases(sndBank);

        DumpSoundData(context, sndBank);
    }
}; // namespace T6::sound

void AssetDumperSndBank::DumpPool(AssetDumpingContext& context, AssetPool<SndBank>* pool)
{
    for (const auto* assetInfo : *pool)
    {
        if (!assetInfo->m_name.empty() && assetInfo->m_name[0] == ',')
            continue;

        sound::DumpSndBank(context, assetInfo);
    }
}
