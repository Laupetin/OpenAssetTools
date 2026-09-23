#pragma once

#include "Game/IGame.h"

namespace IW4
{
    enum class VariantId : std::uint8_t
    {
        PC32,
        PC64,
    };

    class Game final : public AbstractGame
    {
    public:
        Game();

        [[nodiscard]] GameId GetId() const override;
        [[nodiscard]] const std::string& GetFullName() const override;
        [[nodiscard]] const std::string& GetShortName() const override;
        [[nodiscard]] const std::vector<IGameVariant*>& GetVariants() const override;
    };
} // namespace IW4
