#include "DestructibleCommon.h"

#include <format>

namespace destructible
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("destructibledef/{}", assetName);
    }

    std::string GetPieceFileNameForPieceName(const std::string& pieceName)
    {
        return std::format("destructiblepiece/{}", pieceName);
    }

    std::string GetPieceNameForPieceIndex(const std::string& assetName, const size_t pieceIndex)
    {
        return std::format("{}_piece{}", assetName, pieceIndex);
    }
} // namespace destructible
