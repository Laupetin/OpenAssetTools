#pragma once

#include "Game/IGame.h"

#include <string>
#include <vector>

class AutoSearchPaths
{
public:
    AutoSearchPaths() = default;
    virtual ~AutoSearchPaths() = default;
    AutoSearchPaths(const AutoSearchPaths& other) = default;
    AutoSearchPaths(AutoSearchPaths&& other) noexcept = default;
    AutoSearchPaths& operator=(const AutoSearchPaths& other) = default;
    AutoSearchPaths& operator=(AutoSearchPaths&& other) noexcept = default;

    std::vector<std::string> GetSearchPathsForZonePath(const std::string& zonePath) const;

    static AutoSearchPaths* GetForGame(GameId gameId);

protected:
    virtual const std::vector<std::string>& RecognizedZoneDirs() const = 0;
    virtual const std::vector<std::string>& AdditionalSearchPaths() const = 0;
};
