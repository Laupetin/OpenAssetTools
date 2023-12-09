#include "AbstractAliasDumper.h"

#include "ObjContainer/SoundBank/SoundBank.h"

#include <sstream>

using namespace T6::sound;

AbstractAliasDumper::AbstractAliasDumper(const AssetDumpingContext& context)
    : m_context(context)
{
}

bool AbstractAliasDumper::FindSoundBankEntry(const unsigned assetId, SoundAssetBankEntry& entry)
{
    for (const auto* soundBank : SoundBank::Repository)
    {
        if (soundBank->GetEntry(assetId, entry))
            return true;
    }

    return false;
}

std::unique_ptr<std::ostream> AbstractAliasDumper::OpenAliasOutputFile(const SndBank* sndBank, const std::string& extension) const
{
    std::ostringstream ss;

    const char* name;
    if (sndBank->streamAssetBank.zone)
        name = sndBank->streamAssetBank.zone;
    else if (sndBank->loadAssetBank.zone)
        name = sndBank->loadAssetBank.zone;
    else if (sndBank->loadedAssets.zone)
        name = sndBank->loadedAssets.zone;
    else
        name = sndBank->name;

    ss << "soundaliases/" << name << "_aliases" << extension;

    return m_context.OpenAssetFile(ss.str());
}
