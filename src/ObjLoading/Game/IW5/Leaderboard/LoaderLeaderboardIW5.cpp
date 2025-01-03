#include "LoaderLeaderboardIW5.h"

#include "Game/IW5/IW5.h"
#include "JsonLeaderboardDefLoader.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW5;

namespace
{
    class LeaderboardLoader final : public AssetCreator<AssetLeaderboard>
    {
    public:
        LeaderboardLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(std::format("leaderboards/{}.json", assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* leaderboardDef = m_memory.Alloc<LeaderboardDef>();
            leaderboardDef->name = m_memory.Dup(assetName.c_str());

            if (!LoadLeaderboardAsJson(*file.m_stream, *leaderboardDef, m_memory))
            {
                std::cerr << std::format("Failed to load leaderboard \"{}\"\n", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset<AssetLeaderboard>(assetName, leaderboardDef));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace IW5
{
    std::unique_ptr<AssetCreator<AssetLeaderboard>> CreateLeaderboardLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<LeaderboardLoader>(memory, searchPath);
    }
} // namespace IW5
