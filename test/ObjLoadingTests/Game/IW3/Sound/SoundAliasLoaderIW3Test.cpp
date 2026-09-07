#include "Game/IW3/Sound/SoundAliasLoaderIW3.h"

#include "Game/IW3/Sound/LoadedSoundLoaderIW3.h"
#include "Game/IW3/Sound/LoaderSoundCurveIW3.h"
#include "Game/IW3/Sound/SpeakerMapLoaderIW3.h"
#include "Game/IW3/SoundConstantsIW3.h"
#include "SearchPath/MockSearchPath.h"
#include "SearchPath/SearchPathFilesystem.h"
#include "Sound/WavTypes.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <cstring>
#include <format>
#include <sstream>

using namespace IW3;
using namespace std::literals;

namespace
{
    TEST_CASE("SoundAliasLoaderIW3: Loads installed SDK examples", "[.][iw3][sound-alias][sdk]")
    {
        const auto* rawPath = std::getenv("OAT_IW3_SDK_RAW");
        if (!rawPath)
            SKIP("Set OAT_IW3_SDK_RAW to the SDK raw directory");
        SearchPathFilesystem search(rawPath);
        Zone zone("bog", 0, GameId::IW3, GamePlatform::PC);
        MemoryManager memory;
        AssetCreatorCollection creators(zone);
        IgnoredAssetLookup ignored;
        AssetCreationContext context(zone, &creators, &ignored);
        creators.AddAssetCreator(sound_alias::CreateLoaderIW3(memory, search, zone));
        creators.AddAssetCreator(sound::CreateLoadedSoundLoaderIW3(memory, search));
        creators.AddAssetCreator(sound_curve::CreateLoaderIW3(memory, search));
        creators.AddSubAssetCreator(sound_alias::CreateSpeakerMapLoaderIW3(memory, search));
        const auto* fire = context.LoadDependency<AssetSound>("bigfire");
        REQUIRE(fire != nullptr);
        REQUIRE(fire->Asset()->head->volMin == Catch::Approx(0.486f));
        REQUIRE((fire->Asset()->head->flags & SND_ALIAS_FLAG_RANDOM_LOOPING) != 0);
        const auto* voice = context.LoadDependency<AssetSound>("bog_vsq_regroupattank");
        REQUIRE(voice != nullptr);
        REQUIRE(voice->Asset()->head->soundFile->u.loadSnd->name == "null.wav"s);
        REQUIRE(voice->Asset()->head->volMin == Catch::Approx(0.95f));
        REQUIRE((voice->Asset()->head->flags & SND_ALIAS_FLAG_MASTER) != 0);
        const auto* music = context.LoadSubAsset<SubAssetSpeakerMap>("music");
        REQUIRE(music != nullptr);
        REQUIRE(music->Asset()->channelMaps[0][1].speakers[0].levels[0] == Catch::Approx(0.3f));
    }

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
                             "# SDK comment\n\nname,file,vol_min,vol_mod,pitch_min,dist_min,dist_max,channel,type,loop,sequence,subtitle,masterslave\n"
                             "bigfire,fire/Fire_Big_loop02.wav,0.6,na,0.82,7,600,auto,streamed,rlooping,3,\"A subtitle, with comma\",master\n"
                             "bigfire,null.wav,0.2,,1,10,50,mission,loaded,nonlooping,1,,0.5\n");
        f.search.AddFileData("soundaliases/volumemodgroups.def", "VOLUMEMODGROUPS\n// SDK values\nna 0.8100\n");
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
                             "name,file,secondaryaliasname,chainaliasname,volumefalloffcurve,reverb,envelop_min,envelop_max,envelop percentage\n"
                             "first,null.wav,second,,test,fulldrylevel nowetlevel,0.1,0.9,0.4\n"
                             "second,null.wav,,first,test,,,,\n");
        f.search.AddFileData("soundaliases/test.vfcurve", "SNDCURVE\n2\n0 1\n1 0\n");
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

    TEST_CASE("SoundAliasLoaderIW3: Later source overrides matching sequences", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        f.search.AddFileData("soundaliases/a.csv", "name,file,sequence,vol_min\ntest,null.wav,0,0.1\ntest,null.wav,2,0.2\n");
        f.search.AddFileData("soundaliases/z.csv", "name,file,sequence,vol_min\ntest,null.wav,0,0.8\n");
        const auto* info = f.context.LoadDependency<AssetSound>("test");
        REQUIRE(info != nullptr);
        REQUIRE(info->Asset()->count == 2);
        REQUIRE(info->Asset()->head[0].volMin == Catch::Approx(0.8f));
        REQUIRE(info->Asset()->head[1].volMin == Catch::Approx(0.2f));
    }

    TEST_CASE("SoundAliasLoaderIW3: Rejects malformed selected rows", "[iw3][sound-alias][assetloader]")
    {
        Fixture f;
        std::string data;
        SECTION("Duplicate sequence")
        {
            data = "name,file\ntest,null.wav\ntest,null.wav\n";
        }
        SECTION("Invalid float")
        {
            data = "name,file,vol_min\ntest,null.wav,nan\n";
        }
        SECTION("Out of range volume")
        {
            data = "name,file,vol_min\ntest,null.wav,2\n";
        }
        SECTION("Missing loaded sound")
        {
            data = "name,file\ntest,missing.wav\n";
        }
        SECTION("Missing name header")
        {
            data = "file,volume\nnull.wav,1\n";
        }
        SECTION("Missing map")
        {
            data = "name,file,speakermap\ntest,null.wav,missing\n";
        }
        SECTION("Unknown channel")
        {
            data = "name,file,channel\ntest,null.wav,unknown\n";
        }
        SECTION("Unknown volume modifier")
        {
            data = "name,file,vol_mod\ntest,null.wav,unknown\n";
        }
        SECTION("Invalid sequence")
        {
            data = "name,file,sequence\ntest,null.wav,2147483648\n";
        }
        f.search.AddFileData("soundaliases/test.csv", data);
        REQUIRE(f.creators.CreateAsset(ASSET_TYPE_SOUND, "test", f.context).HasFailed());
        REQUIRE(f.zone.m_pools.GetAsset<AssetSound>("test") == nullptr);
    }

} // namespace
