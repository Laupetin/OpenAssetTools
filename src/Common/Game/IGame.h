#pragma once

#include "GameLanguage.h"
#include "Zone/ZoneTypes.h"

#include <cstdint>
#include <optional>
#include <type_traits>
#include <vector>

enum class GameId : std::uint8_t
{
    IW3,
    IW4,
    IW5,
    T5,
    T6,

    COUNT
};

// The full uppercase names are macros in the standard lib
// So unfortunately not usable as values in the enum
enum class GameEndianness : std::uint8_t
{
    /* Little endian */
    LE,
    /* Big endian */
    BE
};

enum class GameWordSize : std::uint8_t
{
    ARCH_32,
    ARCH_64
};

enum class GamePlatform : std::uint8_t
{
    PC,
    XBOX,
    PS3
};

static constexpr const char* GameId_Names[]{
    "IW3",
    "IW4",
    "IW5",
    "T5",
    "T6",
};
static_assert(std::extent_v<decltype(GameId_Names)> == static_cast<unsigned>(GameId::COUNT));

class IGame
{
public:
    IGame() = default;
    virtual ~IGame() = default;
    IGame(const IGame& other) = default;
    IGame(IGame&& other) noexcept = default;
    IGame& operator=(const IGame& other) = default;
    IGame& operator=(IGame&& other) noexcept = default;

    [[nodiscard]] virtual GameId GetId() const = 0;
    [[nodiscard]] virtual const std::string& GetFullName() const = 0;
    [[nodiscard]] virtual const std::string& GetShortName() const = 0;
    [[nodiscard]] virtual const std::vector<GameLanguagePrefix>& GetLanguagePrefixes() const = 0;

    [[nodiscard]] virtual asset_type_t GetAssetTypeCount() const = 0;
    [[nodiscard]] virtual std::optional<const char*> GetAssetTypeName(asset_type_t assetType) const = 0;

    [[nodiscard]] virtual asset_type_t GetSubAssetTypeCount() const = 0;
    [[nodiscard]] virtual std::optional<const char*> GetSubAssetTypeName(asset_type_t subAssetType) const = 0;

    static IGame* GetGameById(GameId gameId);
};
