#include "SndAliasDumperIW3.h"

#include "Sound/CsvWriter.h"
#include "Utils/Logging/Log.h"

#include <format>

using namespace IW3;

namespace
{
    void DumpCsv(const snd_alias_list_t* asset, std::ostream& stream) 
    {
        const CsvWriter writer(stream);

        writer.WriteHeader({"name",
                            "sequence",
                            "file",
                            "vol_min",
                            "vol_max",
                            "vol_mod",
                            "pitch_min",
                            "pitch_max",
                            "dist_min",
                            "dist_max",
                            "channel",
                            "type",
                            "probability",
                            "loop",
                            "masterslave",
                            "loadspec",
                            "subtitle",
                            "compression",
                            "secondaryaliasname",
                            "volumefalloffcurve",
                            "startdelay",
                            "speakermap",
                            "reverb",
                            "lfe percentage",
                            "center percentage",
                            "platform",
                            "envelop_min",
                            "envelop_max",
                            "envelop percentage"});

        for (int i = 0; i < asset->count; i++)
        {
            const snd_alias_t* sndAlias = (asset->head + i);
            if (sndAlias)
            {
                DumpSndAlias(sndAlias, stream);
            }
        }
    }

    void DumpSndAlias(const snd_alias_t* subAsset, std::ostream& stream) 
    {
    }
} // namespace

namespace snd_alias_list
{
    void SndAliasListDumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<snd_alias_list_t>& asset)
    {
        const auto* soundAliasList = asset.Asset();
        const auto assetFile = context.OpenAssetFile(std::format("soundaliases/{}", asset.m_name));

        if (!assetFile)
            return;

        auto& stream = *assetFile;

        switch (static_cast<WavFormat>(loadedSound->sound.info.format))
        {
        case WavFormat::PCM:
            DumpWavPcm(loadedSound, stream);
            break;

        default:
            con::error("Unknown format {} for loaded sound: {}", loadedSound->sound.info.format, loadedSound->name);
            break;
        }
    }
} // namespace sound
