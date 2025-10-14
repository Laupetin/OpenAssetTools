#pragma once

#include "Game/IGame.h"
#include "Utils/ProgressCallback.h"
#include "Zone/ZoneTypes.h"
#include "ZoneLoader.h"

#include <memory>
#include <optional>

struct ZoneLoaderInspectionResult
{
    // The game this zone is created for.
    GameId m_game_id;
    // Whether the zone is meant for a little-endian or big-endian loader.
    GameEndianness m_endianness;
    // Whether the zone is meant for a 32bit or 64bit loader.
    GameWordSize m_word_size;
    // The platform this zone is for.
    GamePlatform m_platform;
    // Whether this zone is confirmed official. False if not official or unknown.
    bool m_is_official;
    // Whether this zone contains a signature confirming the identity of the creator.
    bool m_is_signed;
    // Whether this zone is encrypted.
    bool m_is_encrypted;
};

class IZoneLoaderFactory
{
public:
    IZoneLoaderFactory() = default;
    virtual ~IZoneLoaderFactory() = default;
    IZoneLoaderFactory(const IZoneLoaderFactory& other) = default;
    IZoneLoaderFactory(IZoneLoaderFactory&& other) noexcept = default;
    IZoneLoaderFactory& operator=(const IZoneLoaderFactory& other) = default;
    IZoneLoaderFactory& operator=(IZoneLoaderFactory&& other) noexcept = default;

    [[nodiscard]] virtual std::optional<ZoneLoaderInspectionResult> InspectZoneHeader(const ZoneHeader& header) const = 0;
    [[nodiscard]] virtual std::unique_ptr<ZoneLoader> CreateLoaderForHeader(const ZoneHeader& header,
                                                                            const std::string& fileName,
                                                                            std::optional<std::unique_ptr<ProgressCallback>> progressCallback) const = 0;

    static const IZoneLoaderFactory* GetZoneLoaderFactoryForGame(GameId game);
};
