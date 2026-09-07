#include "Game/IW3/Sound/SoundAliasLoaderIW3.h"

#include "Game/IW3/Sound/LoadedSoundLoaderIW3.h"
#include "Game/IW3/Sound/LoaderSoundCurveIW3.h"
#include "Game/IW3/Sound/SpeakerMapLoaderIW3.h"
#include "Game/IW3/SoundConstantsIW3.h"
#include "SearchPath/MockSearchPath.h"
#include "Sound/WavTypes.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <format>
#include <sstream>

using namespace IW3;
using namespace std::literals;

namespace
{
    class SoundSearchPath final : public ISearchPath
    {
    public:
        void AddFileData(const std::string& name, std::string data)
        {
            m_names.push_back(name);
            m_files.AddFileData(name, std::move(data));
        }

        SearchPathOpenFile Open(const std::string& name) override
        {
            return m_files.Open(name);
        }

        const std::string& GetPath() override
        {
            return m_files.GetPath();
        }

        void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override
        {
            for (const auto& name : m_names)
                if (name.starts_with(options.m_prefix) && (!options.m_filter_extensions || name.ends_with(options.m_extension)))
                    callback(name);
        }

    private:
        MockSearchPath m_files;
        std::vector<std::string> m_names;
    };

    std::string PcmWav()
    {
        const WavHeader header{
            WAV_CHUNK_ID_RIFF, 40, WAV_WAVE_ID, {WAV_CHUNK_ID_FMT, 16},
               {WavFormat::PCM, 1, 22050, 44100, 2, 16},
               {WAV_CHUNK_ID_DATA, 4}
        };
        std::string data(reinterpret_cast<const char*>(&header), sizeof(header));
        data.append("\x00\x00\xFF\x7F", 4);
        return data;
    }

    struct Fixture
    {
        SoundSearchPath search;
        Zone zone{"bog", 0, GameId::IW3, GamePlatform::PC};
        MemoryManager memory;
        AssetCreatorCollection creators{zone};
        IgnoredAssetLookup ignored;
        AssetCreationContext context{zone, &creators, &ignored};

        Fixture()
        {
            creators.AddAssetCreator(sound_alias::CreateLoaderIW3(memory, search, zone));
            creators.AddAssetCreator(sound::CreateLoadedSoundLoaderIW3(memory, search));
            creators.AddAssetCreator(sound_curve::CreateLoaderIW3(memory, search));
            creators.AddSubAssetCreator(sound_alias::CreateSpeakerMapLoaderIW3(memory, search));
            search.AddFileData("sound/null.wav", PcmWav());
        }
    };

    TEST_CASE("SoundAliasLoaderIW3: Loads reordered SDK columns and baked volume", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        f.search.AddFileData("soundaliases/common.csv",
                             R"(# SDK comment

name,file,vol_min,vol_mod,pitch_min,dist_min,dist_max,channel,type,loop,sequence,subtitle,masterslave
bigfire,fire/Fire_Big_loop02.wav,0.6,na,0.82,7,600,auto,streamed,rlooping,3,"A subtitle, with comma",master
bigfire,null.wav,0.2,,1,10,50,mission,loaded,nonlooping,1,,0.5
)");
        f.search.AddFileData("soundaliases/volumemodgroups.def",
                             R"(VOLUMEMODGROUPS
// SDK values
na 0.8100
)");
        auto* info = f.context.LoadDependency<AssetSound>("bigfire");
        REQUIRE(info != nullptr);
        const auto* list = info->Asset();
        REQUIRE(list->count == 2);
        const auto& loaded = list->head[0];
        REQUIRE(loaded.soundFile->type == SAT_LOADED);
        REQUIRE(loaded.soundFile->u.loadSnd->name == "null.wav"s);
        REQUIRE(loaded.soundFile->u.loadSnd->sound.info.samples == 2);
        REQUIRE(loaded.soundFile->u.loadSnd->sound.info.rate == 22050);
        REQUIRE(loaded.slavePercentage == 0.5f);
        REQUIRE((loaded.flags & SND_ALIAS_FLAG_SLAVE) != 0);
        const auto& streamed = list->head[1];
        REQUIRE(streamed.sequence == 0); // Stock compiled sequence is reset after sorting.
        REQUIRE(streamed.volMin == Catch::Approx(0.486f));
        REQUIRE(streamed.volMax == Catch::Approx(0.486f));
        REQUIRE(streamed.pitchMin == Catch::Approx(0.82f));
        REQUIRE(streamed.pitchMax == Catch::Approx(0.82f));
        REQUIRE(streamed.soundFile->u.streamSnd.dir == "fire"s);
        REQUIRE(streamed.soundFile->u.streamSnd.name == "Fire_Big_loop02.wav"s);
        REQUIRE((streamed.flags & SND_ALIAS_FLAG_RANDOM_LOOPING) != 0);
        REQUIRE((streamed.flags & SND_ALIAS_FLAG_MASTER) != 0);
        REQUIRE(streamed.subtitle == "A subtitle, with comma"s);
        REQUIRE(streamed.volumeFalloffCurve == loaded.volumeFalloffCurve);
        REQUIRE(streamed.volumeFalloffCurve->filename == ""s);
        REQUIRE(streamed.volumeFalloffCurve->knotCount == 2);
        REQUIRE(streamed.volumeFalloffCurve->knots[1][0] == 1.0f);
        REQUIRE(streamed.speakerMap == loaded.speakerMap);
        REQUIRE(streamed.speakerMap->isDefault);
        REQUIRE(streamed.speakerMap->channelMaps[0][0].speakers[0].levels[0] == 0.5f);
        REQUIRE(streamed.speakerMap->channelMaps[1][1].speakers[1].levels[1] == 1.0f);
        REQUIRE(streamed.speakerMap->channelMaps[1][1].speakers[2].levels[0] == 0.0f);
    }

    TEST_CASE("SoundAliasLoaderIW3: Resolves cyclic alias references and custom curves", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        f.search.AddFileData("soundaliases/voices.csv",
                             R"(name,file,secondaryaliasname,chainaliasname,volumefalloffcurve,reverb,envelop_min,envelop_max,envelop percentage
first,null.wav,second,,test,fulldrylevel nowetlevel,0.1,0.9,0.4
second,null.wav,,first,test,,,,
)");
        f.search.AddFileData("soundaliases/test.vfcurve", R"(SNDCURVE
2
0 1
1 0
)");
        auto* first = f.context.LoadDependency<AssetSound>("first");
        REQUIRE(first != nullptr);
        const auto* second = f.zone.m_pools.GetAsset<AssetSound>("second");
        REQUIRE(second != nullptr);
        REQUIRE(first->Asset()->head->secondaryAliasName == "second"s);
        REQUIRE(second->Asset()->head->chainAliasName == "first"s);
        REQUIRE(first->Asset()->head->volumeFalloffCurve == second->Asset()->head->volumeFalloffCurve);
        REQUIRE(first->Asset()->head->volumeFalloffCurve->filename == "test"s);
        REQUIRE(first->Asset()->head->envelopPercentage == Catch::Approx(0.4f));
        REQUIRE((first->Asset()->head->flags & SND_ALIAS_FLAG_NO_WET_LEVEL) != 0);
    }

    TEST_CASE("SoundAliasLoaderIW3: Imports all aliases from a named CSV", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        f.search.AddFileData("soundaliases/test.csv",
                             R"(name,file,sequence
first,null.wav,0
second,null.wav,0
first,null.wav,1
)");
        f.search.AddFileData("soundaliases/other.csv",
                             R"(name,file
unrelated,null.wav
)");
        REQUIRE(f.creators.CreateAsset(ASSET_TYPE_SOUND, "test.csv", f.context).HasBeenSuccessful());
        const auto* first = f.zone.m_pools.GetAsset<AssetSound>("first");
        const auto* second = f.zone.m_pools.GetAsset<AssetSound>("second");
        REQUIRE(first != nullptr);
        REQUIRE(second != nullptr);
        REQUIRE(first->Asset()->count == 2);
        REQUIRE(second->Asset()->count == 1);
        REQUIRE(f.zone.m_pools.GetAsset<AssetSound>("unrelated") == nullptr);
    }

    TEST_CASE("SoundAliasLoaderIW3: Later source overrides matching sequences", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        f.search.AddFileData("soundaliases/a.csv", R"(name,file,sequence,vol_min
test,null.wav,0,0.1
test,null.wav,2,0.2
)");
        f.search.AddFileData("soundaliases/z.csv", R"(name,file,sequence,vol_min
test,null.wav,0,0.8
)");
        const auto* info = f.context.LoadDependency<AssetSound>("test");
        REQUIRE(info != nullptr);
        REQUIRE(info->Asset()->count == 2);
        REQUIRE(info->Asset()->head[0].volMin == Catch::Approx(0.8f));
        REQUIRE(info->Asset()->head[1].volMin == Catch::Approx(0.2f));
    }

    TEST_CASE("SoundAliasLoaderIW3: Reordered columns preserve the SDK blank-first-column rule", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        f.search.AddFileData("soundaliases/reordered.csv",
                             R"(sequence,name,file,type
0,enabled,a.wav,streamed
,disabled,b.wav,streamed
)");
        REQUIRE(f.creators.CreateAsset(ASSET_TYPE_SOUND, "reordered.csv", f.context).HasBeenSuccessful());
        const auto* enabled = f.zone.m_pools.GetAsset<AssetSound>("enabled");
        REQUIRE(enabled != nullptr);
        REQUIRE(enabled->Asset()->count == 1);
        REQUIRE(enabled->Asset()->head->soundFile->type == SAT_STREAMED);
        REQUIRE(enabled->Asset()->head->soundFile->u.streamSnd.name == "a.wav"s);
        REQUIRE(f.zone.m_pools.GetAsset<AssetSound>("disabled") == nullptr);
    }

    TEST_CASE("SoundAliasLoaderIW3: Preserves relative probability weights", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        f.search.AddFileData("soundaliases/weights.csv",
                             R"(name,file,type,sequence,probability
weighted,a.wav,streamed,0,2
weighted,b.wav,streamed,1,1
weighted,c.wav,streamed,2,0
)");
        const auto* info = f.context.LoadDependency<AssetSound>("weighted");
        REQUIRE(info != nullptr);
        REQUIRE(info->Asset()->count == 3);
        REQUIRE(info->Asset()->head[0].probability == 2.0f);
        REQUIRE(info->Asset()->head[1].probability == 1.0f);
        REQUIRE(info->Asset()->head[2].probability == 0.0f);
    }

    TEST_CASE("SoundAliasLoaderIW3: Rejects invalid probability weights", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        std::string probability;
        SECTION("Negative weight")
        {
            probability = "-1";
        }
        SECTION("NaN weight")
        {
            probability = "nan";
        }
        SECTION("Infinite weight")
        {
            probability = "inf";
        }
        f.search.AddFileData("soundaliases/weights.csv", std::format("name,file,type,probability\nweighted,a.wav,streamed,{}\n", probability));
        REQUIRE(f.creators.CreateAsset(ASSET_TYPE_SOUND, "weighted", f.context).HasFailed());
        REQUIRE(f.zone.m_pools.GetAsset<AssetSound>("weighted") == nullptr);
    }

    TEST_CASE("SoundAliasLoaderIW3: Rejects malformed selected rows", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        std::string data;
        SECTION("Duplicate sequence")
        {
            data = R"(name,file
test,null.wav
test,null.wav
)";
        }
        SECTION("Invalid float")
        {
            data = R"(name,file,vol_min
test,null.wav,nan
)";
        }
        SECTION("Out of range volume")
        {
            data = R"(name,file,vol_min
test,null.wav,2
)";
        }
        SECTION("Missing loaded sound")
        {
            data = R"(name,file
test,missing.wav
)";
        }
        SECTION("Missing name header")
        {
            data = R"(file,volume
null.wav,1
)";
        }
        SECTION("Missing map")
        {
            data = R"(name,file,speakermap
test,null.wav,missing
)";
        }
        SECTION("Unknown channel")
        {
            data = R"(name,file,channel
test,null.wav,unknown
)";
        }
        SECTION("Unknown volume modifier")
        {
            data = R"(name,file,vol_mod
test,null.wav,unknown
)";
        }
        SECTION("Invalid sequence")
        {
            data = R"(name,file,sequence
test,null.wav,2147483648
)";
        }
        f.search.AddFileData("soundaliases/test.csv", data);
        REQUIRE(f.creators.CreateAsset(ASSET_TYPE_SOUND, "test", f.context).HasFailed());
        REQUIRE(f.zone.m_pools.GetAsset<AssetSound>("test") == nullptr);
    }

} // namespace
