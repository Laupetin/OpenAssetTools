#pragma once

#include "IGame.h"

#include <cstdint>
#include <string>

/*
 * A game variant is a specific game version or platform files are created for.
 * This determines the layout of the structs, endianness/wordsize in the zone and informs about the target platform.
 */
enum class GameVariantId : std::uint8_t
{
    IW3_PC,
    IW3_XBOX,
    IW4_PC32,
    IW4_PC64,
    IW4_XBOX,
    IW5_PC32,
    IW5_PC64,
    IW5_XBOX,
    QOS_PC,
    T4_PC,
    T5_PC,
    T6_PC,
    T6_XBOX,
    T6_PS3,
    T6_WIIU,

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
    PS3,
    WIIU
};

class IGameVariant
{
public:
    IGameVariant() = default;
    virtual ~IGameVariant() = default;
    IGameVariant(const IGameVariant& other) = default;
    IGameVariant(IGameVariant&& other) noexcept = default;
    IGameVariant& operator=(const IGameVariant& other) = default;
    IGameVariant& operator=(IGameVariant&& other) noexcept = default;

    static IGameVariant* GetVariantById(GameVariantId variantId);

    [[nodiscard]] virtual GameVariantId GetId() const = 0;
    [[nodiscard]] virtual GameId GetGameId() const = 0;
    [[nodiscard]] virtual const std::string& GetName() const = 0;
    [[nodiscard]] virtual GameEndianness GetEndianness() const = 0;
    [[nodiscard]] virtual GameWordSize GetWordSize() const = 0;
    [[nodiscard]] virtual GamePlatform GetPlatform() const = 0;
};

class GameVariant : public IGameVariant
{
public:
    GameVariant(GameVariantId id, GameId gameId, std::string name, GameEndianness endianness, GameWordSize wordSize, GamePlatform platform);

    [[nodiscard]] GameVariantId GetId() const override;
    [[nodiscard]] GameId GetGameId() const override;
    [[nodiscard]] const std::string& GetName() const override;
    [[nodiscard]] GameEndianness GetEndianness() const override;
    [[nodiscard]] GameWordSize GetWordSize() const override;
    [[nodiscard]] GamePlatform GetPlatform() const override;

private:
    GameVariantId m_id;
    GameId m_game_id;
    std::string m_name;
    GameEndianness m_endianness;
    GameWordSize m_word_size;
    GamePlatform m_platform;
};
