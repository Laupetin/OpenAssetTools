#pragma once
#include "Game/IGame.h"

namespace T6
{
    class Game final : public IGame
    {
    public:
        [[nodiscard]] GameId GetId() const override;
        [[nodiscard]] const std::string& GetFullName() const override;
        [[nodiscard]] const std::string& GetShortName() const override;
        [[nodiscard]] const std::vector<GameLanguagePrefix>& GetLanguagePrefixes() const override;
    };
} // namespace T6
