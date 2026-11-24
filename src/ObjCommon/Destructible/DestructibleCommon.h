#pragma once

#include <cstddef>
#include <string>

namespace destructible
{
    std::string GetFileNameForAssetName(const std::string& assetName);
    std::string GetPieceFileNameForPieceName(const std::string& pieceName);
    std::string GetPieceNameForPieceIndex(const std::string& assetName, size_t pieceIndex);
} // namespace destructible
