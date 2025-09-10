#include "SndDriverGlobalsDumperT6.h"

#include "Csv/CsvStream.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "Utils/Logging/Log.h"

#include <format>

using namespace T6;

namespace
{
    const std::string GROUPS_HEADERS[]{
        "name",
        "attenuationSp",
        "attenuationMp",
        "category",
        "parent",
        "id",
    };

    const std::string GROUPS_CATEGORIES[]{
        "sfx",
        "music",
        "void",
        "ui",
        "cinematic",
        "id",
    };

    const std::string CURVE_HEADERS[]{
        "name",
        "x0",
        "y0",
        "x1",
        "y1",
        "x2",
        "y2",
        "x3",
        "y3",
        "x4",
        "y4",
        "x5",
        "y5",
        "x6",
        "y6",
        "x7",
        "y7",
        "id",
    };

    const std::string PAN_HEADERS[]{
        "name",
        "front",
        "back",
        "center",
        "lfe",
        "left",
        "right",
        "id",
    };

    const std::string MASTER_HEADERS[]{
        "name",      "lowE",      "lowG",      "lowF",      "lowQ",      "peak1E",     "peak1G",    "peak1F",     "peak1Q",     "peak2E",  "peak2G",
        "peak2F",    "peak2Q",    "hiE",       "hiG",       "hiF",       "hiQ",        "eqG",       "compE",      "compPG",     "compMG",  "compT",
        "compR",     "compTA",    "compTR",    "limitE",    "limitPG",   "limitMG",    "limitT",    "limitR",     "limitTA",    "limitTR", "busReverbG",
        "busFxG",    "busVoiceG", "busPfutzG", "busHdrfxG", "busUiG",    "busMusicG",  "busMovieG", "busVcsG",    "busReverbE", "busFxE",  "busVoiceE",
        "busPfutzE", "busHdrfxE", "busUiE",    "busMusicE", "busMovieE", "hdrfxCompE", "voiceEqE",  "voiceCompE", "id",
    };

    const std::string SIDECHAIN_HEADERS[]{
        "name",
        "g",
        "f",
        "q",
        "ta",
        "tr",
        "tf",
        "id",
    };

    const std::string FUTZ_HEADERS[]{
        "name",
        "bpfF",
        "bpfQ",
        "lsG",
        "lsF",
        "lsQ",
        "dist",
        "preG",
        "postG",
        "th",
        "tg",
        "clippre",
        "clippost",
        "blend",
        "startAliasId",
        "stopAliasId",
        "loopAliasId",
        "id",
    };

    class Internal
    {
    public:
        explicit Internal(AssetDumpingContext& context)
            : m_context(context)
        {
        }

        void DumpPool(AssetPool<SndDriverGlobals>* pool)
        {
            for (const auto* assetInfo : *pool)
            {
                if (!assetInfo->m_name.empty() && assetInfo->m_name[0] == ',')
                    continue;

                DumpSndDriverGlobals(assetInfo);
            }
        }

    private:
        std::unique_ptr<std::ostream> OpenAssetFile(const std::string& filename)
        {
            auto outputFile = this->m_context.OpenAssetFile(filename);
            if (outputFile == nullptr)
                con::error("Failed to open sound driver globals output file for: \"{}\"", filename);

            return outputFile;
        }

        static void WriteFileHeader(CsvOutputStream& stream, const std::string* headers, size_t count)
        {
            for (auto i = 0u; i < count; i++)
            {
                stream.WriteColumn(headers[i]);
            }

            stream.NextRow();
        }

        void DumpSndVolumesGroups(const SndVolumeGroup* groups, const size_t count)
        {
            const auto outputFile = this->OpenAssetFile("soundbank/globals/group.csv");

            if (outputFile != nullptr)
            {
                CsvOutputStream csvStream(*outputFile);
                WriteFileHeader(csvStream, GROUPS_HEADERS, std::extent_v<decltype(GROUPS_HEADERS)>);

                for (auto i = 0u; i < count; i++)
                {
                    const auto& group = groups[i];
                    csvStream.WriteColumn(group.name);
                    csvStream.WriteColumn(std::to_string(group.attenuationSp));
                    csvStream.WriteColumn(std::to_string(group.attenuationMp));
                    csvStream.WriteColumn(GROUPS_CATEGORIES[group.category]);
                    csvStream.WriteColumn(group.parentName);
                    csvStream.WriteColumn(std::to_string(group.id));
                    csvStream.NextRow();
                }
            }
        }

        void DumpSndCurves(const SndCurve* curves, const size_t count)
        {
            const auto outputFile = this->OpenAssetFile("soundbank/globals/curves.csv");

            if (outputFile != nullptr)
            {
                CsvOutputStream csvStream(*outputFile);
                WriteFileHeader(csvStream, CURVE_HEADERS, std::extent_v<decltype(CURVE_HEADERS)>);

                for (auto i = 0u; i < count; i++)
                {
                    const auto& curve = curves[i];
                    csvStream.WriteColumn(curve.name);

                    for (auto j = 0u; j < 8; j++)
                    {
                        csvStream.WriteColumn(std::to_string(curve.points[j].x));
                        csvStream.WriteColumn(std::to_string(curve.points[j].y));
                    }

                    csvStream.WriteColumn(std::to_string(curve.id));

                    csvStream.NextRow();
                }
            }
        }

        void DumpSndPans(const SndPan* pans, const size_t count)
        {
            const auto outputFile = this->OpenAssetFile("soundbank/globals/pan.csv");

            if (outputFile != nullptr)
            {
                CsvOutputStream csvStream(*outputFile);
                WriteFileHeader(csvStream, PAN_HEADERS, std::extent_v<decltype(PAN_HEADERS)>);

                for (auto i = 0u; i < count; i++)
                {
                    const auto& pan = pans[i];
                    csvStream.WriteColumn(pan.name);
                    csvStream.WriteColumn(std::to_string(pan.front));
                    csvStream.WriteColumn(std::to_string(pan.back));
                    csvStream.WriteColumn(std::to_string(pan.center));
                    csvStream.WriteColumn(std::to_string(pan.lfe));
                    csvStream.WriteColumn(std::to_string(pan.left));
                    csvStream.WriteColumn(std::to_string(pan.right));
                    csvStream.WriteColumn(std::to_string(pan.id));
                    csvStream.NextRow();
                }
            }
        }

        void DumpSndDuckGroups(const SndDuckGroup* duckGroups, const size_t count)
        {
            const auto outputFile = this->OpenAssetFile("soundbank/globals/duck_groups.csv");

            if (outputFile != nullptr)
            {
                CsvOutputStream csvStream(*outputFile);
                csvStream.WriteColumn("name");
                csvStream.WriteColumn("id");
                csvStream.NextRow();

                for (auto i = 0u; i < count; i++)
                {
                    const auto& duckGroup = duckGroups[i];
                    csvStream.WriteColumn(duckGroup.name);
                    csvStream.WriteColumn(std::to_string(duckGroup.id));
                    csvStream.NextRow();
                }
            }
        }

        void DumpSndMasters(const SndMaster* masters, const size_t count)
        {
            const auto outputFile = this->OpenAssetFile("soundbank/globals/master.csv");

            if (outputFile != nullptr)
            {
                CsvOutputStream csvStream(*outputFile);
                WriteFileHeader(csvStream, MASTER_HEADERS, std::extent_v<decltype(MASTER_HEADERS)>);

                for (auto i = 0u; i < count; i++)
                {
                    const auto& master = masters[i];
                    csvStream.WriteColumn(master.name);
                    csvStream.WriteColumn(std::to_string(master.lowE));
                    csvStream.WriteColumn(std::to_string(master.lowG));
                    csvStream.WriteColumn(std::to_string(master.lowF));
                    csvStream.WriteColumn(std::to_string(master.lowQ));
                    csvStream.WriteColumn(std::to_string(master.peak1E));
                    csvStream.WriteColumn(std::to_string(master.peak1G));
                    csvStream.WriteColumn(std::to_string(master.peak1F));
                    csvStream.WriteColumn(std::to_string(master.peak1Q));
                    csvStream.WriteColumn(std::to_string(master.peak2E));
                    csvStream.WriteColumn(std::to_string(master.peak2G));
                    csvStream.WriteColumn(std::to_string(master.peak2F));
                    csvStream.WriteColumn(std::to_string(master.peak2Q));
                    csvStream.WriteColumn(std::to_string(master.hiE));
                    csvStream.WriteColumn(std::to_string(master.hiG));
                    csvStream.WriteColumn(std::to_string(master.hiF));
                    csvStream.WriteColumn(std::to_string(master.hiQ));
                    csvStream.WriteColumn(std::to_string(master.eqG));
                    csvStream.WriteColumn(std::to_string(master.compE));
                    csvStream.WriteColumn(std::to_string(master.compPG));
                    csvStream.WriteColumn(std::to_string(master.compMG));
                    csvStream.WriteColumn(std::to_string(master.compT));
                    csvStream.WriteColumn(std::to_string(master.compR));
                    csvStream.WriteColumn(std::to_string(master.compTA));
                    csvStream.WriteColumn(std::to_string(master.compTR));
                    csvStream.WriteColumn(std::to_string(master.limitE));
                    csvStream.WriteColumn(std::to_string(master.limitPG));
                    csvStream.WriteColumn(std::to_string(master.limitMG));
                    csvStream.WriteColumn(std::to_string(master.limitT));
                    csvStream.WriteColumn(std::to_string(master.limitR));
                    csvStream.WriteColumn(std::to_string(master.limitTA));
                    csvStream.WriteColumn(std::to_string(master.limitTR));
                    csvStream.WriteColumn(std::to_string(master.busReverbG));
                    csvStream.WriteColumn(std::to_string(master.busFxG));
                    csvStream.WriteColumn(std::to_string(master.busVoiceG));
                    csvStream.WriteColumn(std::to_string(master.busPfutzG));
                    csvStream.WriteColumn(std::to_string(master.busHdrfxG));
                    csvStream.WriteColumn(std::to_string(master.busUiG));
                    csvStream.WriteColumn(std::to_string(master.busMusicG));
                    csvStream.WriteColumn(std::to_string(master.busMovieG));
                    csvStream.WriteColumn(std::to_string(master.busVcsG));
                    csvStream.WriteColumn(std::to_string(master.busReverbE));
                    csvStream.WriteColumn(std::to_string(master.busFxE));
                    csvStream.WriteColumn(std::to_string(master.busVoiceE));
                    csvStream.WriteColumn(std::to_string(master.busPfutzE));
                    csvStream.WriteColumn(std::to_string(master.busHdrfxE));
                    csvStream.WriteColumn(std::to_string(master.busUiE));
                    csvStream.WriteColumn(std::to_string(master.busMusicE));
                    csvStream.WriteColumn(std::to_string(master.busMovieE));
                    csvStream.WriteColumn(std::to_string(master.hdrfxCompE));
                    csvStream.WriteColumn(std::to_string(master.voiceEqE));
                    csvStream.WriteColumn(std::to_string(master.voiceCompE));
                    csvStream.WriteColumn(std::to_string(master.id));
                    csvStream.NextRow();
                }
            }
        }

        void DumpSndSidechainDucks(const SndSidechainDuck* sidechains, const size_t count)
        {
            const auto outputFile = this->OpenAssetFile("soundbank/globals/sidechain_duck.csv");

            if (outputFile != nullptr)
            {
                CsvOutputStream csvStream(*outputFile);
                WriteFileHeader(csvStream, SIDECHAIN_HEADERS, std::extent_v<decltype(SIDECHAIN_HEADERS)>);

                for (auto i = 0u; i < count; i++)
                {
                    const auto& sidechain = sidechains[i];
                    csvStream.WriteColumn(sidechain.name);
                    csvStream.WriteColumn(std::to_string(sidechain.g));
                    csvStream.WriteColumn(std::to_string(sidechain.f));
                    csvStream.WriteColumn(std::to_string(sidechain.q));
                    csvStream.WriteColumn(std::to_string(sidechain.ta));
                    csvStream.WriteColumn(std::to_string(sidechain.tr));
                    csvStream.WriteColumn(std::to_string(sidechain.tf));
                    csvStream.WriteColumn(std::to_string(sidechain.id));
                    csvStream.NextRow();
                }
            }
        }

        void DumpSndFutz(const SndFutz* futzes, const size_t count)
        {
            const auto outputFile = this->OpenAssetFile("soundbank/globals/futz.csv");

            if (outputFile != nullptr)
            {
                CsvOutputStream csvStream(*outputFile);
                WriteFileHeader(csvStream, FUTZ_HEADERS, std::extent_v<decltype(FUTZ_HEADERS)>);

                for (auto i = 0u; i < count; i++)
                {
                    const auto& futz = futzes[i];
                    csvStream.WriteColumn(futz.name);
                    csvStream.WriteColumn(std::to_string(futz.bpfF));
                    csvStream.WriteColumn(std::to_string(futz.bpfQ));
                    csvStream.WriteColumn(std::to_string(futz.lsG));
                    csvStream.WriteColumn(std::to_string(futz.lsF));
                    csvStream.WriteColumn(std::to_string(futz.lsQ));
                    csvStream.WriteColumn(std::to_string(futz.dist));
                    csvStream.WriteColumn(std::to_string(futz.preG));
                    csvStream.WriteColumn(std::to_string(futz.postG));
                    csvStream.WriteColumn(std::to_string(futz.th));
                    csvStream.WriteColumn(std::to_string(futz.tg));
                    csvStream.WriteColumn(std::to_string(futz.clippre));
                    csvStream.WriteColumn(std::to_string(futz.clippost));
                    csvStream.WriteColumn(std::to_string(futz.blend));
                    csvStream.WriteColumn(std::to_string(futz.startAliasId));
                    csvStream.WriteColumn(std::to_string(futz.stopAliasId));
                    csvStream.WriteColumn(std::to_string(futz.loopAliasId));
                    csvStream.WriteColumn(std::to_string(futz.id));
                    csvStream.NextRow();
                }
            }
        }

        void DumpSndDriverGlobals(const XAssetInfo<SndDriverGlobals>* sndDriverGlobalsInfo)
        {
            const auto* sndDriverGlobals = sndDriverGlobalsInfo->Asset();

            DumpSndVolumesGroups(sndDriverGlobals->groups, sndDriverGlobals->groupCount);
            DumpSndCurves(sndDriverGlobals->curves, sndDriverGlobals->curveCount);
            DumpSndPans(sndDriverGlobals->pans, sndDriverGlobals->panCount);
            DumpSndDuckGroups(sndDriverGlobals->duckGroups, sndDriverGlobals->duckGroupCount);
            // DumpSndContexts(sndDriverGlobals->contexts, sndDriverGlobals->contextCount);
            DumpSndMasters(sndDriverGlobals->masters, sndDriverGlobals->masterCount);
            DumpSndSidechainDucks(sndDriverGlobals->voiceDucks, sndDriverGlobals->voiceDuckCount);
            DumpSndFutz(sndDriverGlobals->futzes, sndDriverGlobals->futzCount);
        }

        AssetDumpingContext& m_context;
    };
} // namespace

namespace sound
{
    void SndDriverGlobalsDumperT6::DumpPool(AssetDumpingContext& context, AssetPool<SndDriverGlobals>* pool)
    {
        Internal internal(context);
        internal.DumpPool(pool);
    }
} // namespace sound
