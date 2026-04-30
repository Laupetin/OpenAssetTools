#include "SndAliasListDumperIW3.h"

#include "Sound/CsvWriter.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <Game/IW3/Sound/SndAliasListFields.h>

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

        const char* dir = "";

        stream << ',';
        if (subAsset->soundFile && (subAsset->soundFile->type == SAT_STREAMED) && subAsset->soundFile->u.streamSnd.dir && subAsset->soundFile->u.streamSnd.name)
        {
            dir = subAsset->soundFile->u.streamSnd.dir;
            stream << dir << '/' << subAsset->soundFile->u.streamSnd.name;
        }
        else if (subAsset->soundFile && (subAsset->soundFile->type == SAT_LOADED) && subAsset->soundFile->u.loadSnd && subAsset->soundFile->u.loadSnd->name)
        {
            stream << subAsset->soundFile->u.loadSnd->name;
        }

        stream << ',' << subAsset->volMin << ',' << subAsset->volMax;

        stream << ',';
        // TODO: Try to finish heuristic
        //if (strstr(dir, "footsteps/"))
        //{
        //    stream << "footstep";

        //    // TODO: Expand heuristic to check platform for mp.
        //    // 
        //    // If channel is body & platform is !all_mp, set to 'footstep'
        //    // If channel is body & platform is none or all_mp, set to 'mpfootstep'
        //    // If channel is body2d, set to 'footstepplr' 
        //}
        //else if (strstr(dir, "battlechatter/"))
        //{
        //    if (strstr(dir, "/ab/") || strstr(dir, "/ru/"))
        //    {
        //        stream << "chatterax";
        //    }
        //    else if (strstr(dir, "/uk/") || strstr(dir, "/us/"))
        //    {
        //        stream << "chatteral";
        //    }
        //}
        //else if (strstr(dir, "explosions/"))
        //{
        //    stream << "explosion";
        //}

        //
        //"ac130",
        //"airlift",
        //"aftermath",
        //"ambience",
        //"bodyfall",
        //"",
        //"",
        //"element",
        //"emitter",
        //"",
        //"foley",
        //"",
        //"",
        //"gear",
        //"impact",
        //"impactmp",
        //"interface",
        //"mpfoley",
        //"",
        //"mpgear",
        //"music",
        //"physics",
        //"quiet",
        //"reactional",
        //"reactionax",
        //"vehicle",
        //"voiceover",
        //"whizby",
        //"wpnai",
        //"wpnplyr",
        //"na",
        //"max",

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

        int channelIndex = (subAsset->flags >> 8) & 0x3F;
        stream << ',';
        if (channelIndex < std::size(snd_alias_channel_names))
        {
            stream << snd_alias_channel_names[channelIndex];
        }
        else
        {
            stream << "auto";
        }

        // TODO; Verify this
        int type = (subAsset->flags >> 6) & 3;
        stream << ',';
        switch (type)
        {
        case 1:
            // 'loaded' type is default, don't write
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

        stream << ',';
        if (subAsset->subtitle)
        {
            stream << subAsset->subtitle;
        }

        // TODO: See if we can deduce compression
        stream << ','; // << subAsset->compression

        stream << ',';
        if (subAsset->secondaryAliasName)
        {
            stream << subAsset->secondaryAliasName;
        }

        stream << ',';
        if (subAsset->volumeFalloffCurve && subAsset->volumeFalloffCurve->filename)
        {
            stream << subAsset->volumeFalloffCurve->filename;
        }

        stream << ',';
        if (subAsset->startDelay != 0)
        {
            stream << subAsset->startDelay;
        }

        stream << ',';
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
