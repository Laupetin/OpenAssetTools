#pragma once

#include <string>

namespace destructible_def
{
    std::string GetFileNameForDestructibleDef(const std::string& assetName);
    std::string GetFileNameForDestructiblePiece(const std::string& assetName, const int pieceIndex);
} // namespace destructible_def
