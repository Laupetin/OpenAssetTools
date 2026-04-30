#include "SndAliasListDumperIW3.h"

#include "Sound/CsvWriter.h"
#include "Utils/Logging/Log.h"

#include <format>

using namespace IW3;

namespace
{
    void DumpSndAlias(const snd_alias_t* subAsset, std::ostream& stream, int sequence = 0)
    {
        if (subAsset->aliasName)
        {
            stream << subAsset->aliasName;
        }

        stream << ',';
        if (subAsset->sequence != 0)
        {
            stream << subAsset->sequence;
        }
        else
        {
            stream << sequence;
        }

        stream << ',';
        if (subAsset->soundFile && (subAsset->soundFile->type == SAT_STREAMED) && subAsset->soundFile->u.streamSnd.dir && subAsset->soundFile->u.streamSnd.name)
        {
            stream << subAsset->soundFile->u.streamSnd.dir << '/' << subAsset->soundFile->u.streamSnd.name;
        }
        else if (subAsset->soundFile && (subAsset->soundFile->type == SAT_LOADED) && subAsset->soundFile->u.loadSnd && subAsset->soundFile->u.loadSnd->name)
        {
            stream << subAsset->soundFile->u.loadSnd->name;
        }

        stream << ',' << subAsset->volMin << ',' << subAsset->volMax;

        // TODO: See if we can deduce volMod
        stream << ','; // << subAsset->volMod

        stream << ',';
        if (subAsset->pitchMin != 1.0f)
        {
            stream << subAsset->pitchMin;
        }

        stream << ',';
        if (subAsset->pitchMax != 1.0f)
        {
            stream << subAsset->pitchMax;
        }

        stream << ',' << subAsset->distMin << ',' << subAsset->distMax;

        // TODO: Figure out if this is actually the channel or something different
        int channelIndex = (subAsset->flags >> 8) & 0x3F;
        stream << ',';
        switch (channelIndex)
        {
        case 0:
            stream << "menu";
            break;
        case 1:
            stream << "weapon";
            break;
        case 2:
            stream << "voice";
            break;
        case 3:
            stream << "item";
            break;
        case 4:
            stream << "body";
            break;
        case 5:
            stream << "local";
            break;
        case 6:
            stream << "music";
            break;
        case 7:
            stream << "announcer";
            break;
        default:
            stream << "auto";
            break;
        }

        // TODO; Verify this
        int type = (subAsset->flags >> 6) & 3;
        stream << ',';
        switch (type)
        {
        case 1:
            stream << "loaded";
            break;
        case 2:
        case 3:
            stream << "streamed";
            break;
        default:
            stream << "primed";
            break;
        }

        stream << ',';
        if (subAsset->probability != 1.0f)
        {
            stream << subAsset->probability;
        }

        // TODO: This seems wrong
        bool looping = subAsset->flags & 1;
        stream << ',';
        if (looping)
        {
            stream << "looping";
        }

        // TODO: This seems wrong
        bool master = subAsset->flags & 2;
        stream << ',';
        if (master)
        {
            stream << "master";
        }
        else if (subAsset->slavePercentage > 0.0f)
        {
            stream << subAsset->slavePercentage;
        }

        // TODO: See if we can deduce loadspec
        stream << ','; // << subAsset->loadSpec

        if (subAsset->subtitle)
        {
            stream << subAsset->subtitle;
        }

        // TODO: See if we can deduce compression
        stream << ','; // << subAsset->compression

        if (subAsset->secondaryAliasName)
        {
            stream << subAsset->secondaryAliasName;
        }

        if (subAsset->volumeFalloffCurve && subAsset->volumeFalloffCurve->filename)
        {
            stream << subAsset->volumeFalloffCurve->filename;
        }

        stream << ',';
        if (subAsset->startDelay != 0)
        {
            stream << subAsset->startDelay;
        }

        if (subAsset->speakerMap && subAsset->speakerMap->name)
        {
            stream << subAsset->speakerMap->name;
        }

        // TODO: See if we can deduce reverb
        stream << ','; // << subAsset->reverb

        stream << ',';
        if (subAsset->lfePercentage != 0)
        {
            stream << subAsset->lfePercentage;
        }

        stream << ',';
        if (subAsset->centerPercentage != 0)
        {
            stream << subAsset->centerPercentage;
        }

        // TODO: See if we can deduce platform
        stream << ','; // << subAsset->platform

        stream << ',';
        if (subAsset->envelopMin != 0)
        {
            stream << subAsset->envelopMin;
        }

        stream << ',';
        if (subAsset->envelopMax != 0)
        {
            stream << subAsset->envelopMax;
        }

        stream << ',';
        if (subAsset->envelopPercentage != 0)
        {
            stream << subAsset->envelopPercentage;
        }
    }

    void DumpSndAliasList(const snd_alias_list_t* asset, std::ostream& stream)
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

        stream << "\r\n\r\n";

        for (int i = 0; i < asset->count; i++)
        {
            const snd_alias_t* sndAlias = (asset->head + i);
            if (sndAlias)
            {
                DumpSndAlias(sndAlias, stream, i + 1);

                stream << "\r\n";
            }
        }
    }
} // namespace

namespace snd_alias_list
{
    void SndAliasListDumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<snd_alias_list_t>& asset)
    {
        const auto* soundAliasList = asset.Asset();
        const auto assetFile = context.OpenAssetFile(std::format("soundaliases/{}", asset.m_name + ".csv"));

        if (!assetFile)
            return;

        auto& stream = *assetFile;

        DumpSndAliasList(soundAliasList, stream);
    }
} // namespace snd_alias_list
