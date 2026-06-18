#include "AutoSearchPaths.h"

#include "IW3/AutoSearchPathsIW3.h"
#include "IW4/AutoSearchPathsIW4.h"
#include "IW5/AutoSearchPathsIW5.h"
#include "T4/AutoSearchPathsT4.h"
#include "T5/AutoSearchPathsT5.h"
#include "T6/AutoSearchPathsT6.h"
#include "Utils/StringUtils.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <optional>
#include <utility>

namespace fs = std::filesystem;

namespace
{
    std::optional<std::string> FindGameRootFolder(const std::string& zoneParentPath, const std::vector<std::string>& zoneDirs)
    {
        std::string lowerZoneParentPath(zoneParentPath);
        utils::MakeStringLowerCase(lowerZoneParentPath);

        for (const auto& dir : zoneDirs)
        {
            std::string normalizedDir(dir);
            utils::MakeStringLowerCase(normalizedDir);

            if (lowerZoneParentPath.ends_with(normalizedDir) && lowerZoneParentPath[lowerZoneParentPath.size() - dir.size() - 1] == '/')
                return zoneParentPath.substr(0, zoneParentPath.size() - dir.size() - 1);
        }

        return std::nullopt;
    }
} // namespace

std::vector<std::string> AutoSearchPaths::GetSearchPathsForZonePath(const std::string& zonePath) const
{
    auto folderName = fs::absolute(fs::path(zonePath)).parent_path().string();
    std::ranges::replace(folderName, '\\', '/');

    const auto maybeGameRootFolder = FindGameRootFolder(folderName, RecognizedZoneDirs());
    if (!maybeGameRootFolder)
        return {folderName};

    std::vector<std::string> result;
    const fs::path gameRootFolderPath(*maybeGameRootFolder);

    for (const auto& dir : RecognizedZoneDirs())
    {
        auto dirPath = fs::weakly_canonical(gameRootFolderPath / dir);

        if (fs::is_directory(dirPath))
            result.emplace_back(dirPath.string());
    }

    for (const auto& dir : AdditionalSearchPaths())
    {
        auto dirPath = fs::weakly_canonical(gameRootFolderPath / dir);

        if (fs::is_directory(dirPath))
            result.emplace_back(dirPath.string());
    }

    return result;
}

AutoSearchPaths* AutoSearchPaths::GetForGame(GameId gameId)
{
    static AutoSearchPaths* autoSearchPaths[]{
        new AutoSearchPathsIW3(),
        new AutoSearchPathsIW4(),
        new AutoSearchPathsIW5(),
        new AutoSearchPathsT4(),
        new AutoSearchPathsT5(),
        new AutoSearchPathsT6(),
    };
    static_assert(std::extent_v<decltype(autoSearchPaths)> == static_cast<unsigned>(GameId::COUNT));
    assert(static_cast<unsigned>(gameId) < static_cast<unsigned>(GameId::COUNT));

    return autoSearchPaths[std::to_underlying(gameId)];
}
