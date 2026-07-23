#include "DestructibleDefCommon.h"

#include <format>

namespace destructible_def
{
    std::string GetFileNameForDestructibleDef(const std::string& assetName)
    {
        return std::format("destructibledef/{}", assetName);
    }

    std::string GetFileNameForDestructiblePiece(const std::string& assetName, const int pieceIndex)
    {
        return std::format("destructiblepiece/{}_{}", assetName, pieceIndex);
    }
} // namespace destructible_def
