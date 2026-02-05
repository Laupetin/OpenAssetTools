#pragma once
#include "Game/IGame.h"

namespace IW5
{
    class Game final : public IGame
    {
    public:
        [[nodiscard]] GameId GetId() const override;
        [[nodiscard]] const std::string& GetFullName() const override;
        [[nodiscard]] const std::string& GetShortName() const override;
        [[nodiscard]] const std::vector<GameLanguagePrefix>& GetLanguagePrefixes() const override;

        [[nodiscard]] asset_type_t GetAssetTypeCount() const override;
        [[nodiscard]] std::optional<const char*> GetAssetTypeName(asset_type_t assetType) const override;
    };
} // namespace IW5
