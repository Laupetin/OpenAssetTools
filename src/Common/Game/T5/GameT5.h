#pragma once
#include "Game/IGame.h"

namespace T5
{
    class Game final : public IGame
    {
    public:
        [[nodiscard]] GameId GetId() const override;
        [[nodiscard]] const std::string& GetFullName() const override;
        [[nodiscard]] const std::string& GetShortName() const override;
        void AddZone(Zone* zone) override;
        void RemoveZone(Zone* zone) override;
        [[nodiscard]] const std::vector<Zone*>& GetZones() const override;
        [[nodiscard]] const std::vector<GameLanguagePrefix>& GetLanguagePrefixes() const override;

    private:
        std::vector<Zone*> m_zones;
    };
} // namespace T5
