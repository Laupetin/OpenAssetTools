#include "GameVariant.h"

#include <cassert>
#include <utility>

GameVariant::GameVariant(
    const GameVariantId id, const GameId gameId, std::string name, const GameEndianness endianness, const GameWordSize wordSize, const GamePlatform platform)
    : m_id(id),
      m_game_id(gameId),
      m_name(std::move(name)),
      m_endianness(endianness),
      m_word_size(wordSize),
      m_platform(platform)
{
}

GameVariantId GameVariant::GetId() const
{
    return m_id;
}

GameId GameVariant::GetGameId() const
{
    return m_game_id;
}

const std::string& GameVariant::GetName() const
{
    return m_name;
}

GameEndianness GameVariant::GetEndianness() const
{
    return m_endianness;
}

GameWordSize GameVariant::GetWordSize() const
{
    return m_word_size;
}

GamePlatform GameVariant::GetPlatform() const
{
    return m_platform;
}

IGameVariant* IGameVariant::GetVariantById(const GameVariantId variantId)
{
    static IGameVariant* variants[]{
        new GameVariant(GameVariantId::IW3_PC, GameId::IW3, "pc", GameEndianness::LE, GameWordSize::ARCH_32, GamePlatform::PC),
        new GameVariant(GameVariantId::IW3_XBOX, GameId::IW3, "xbox", GameEndianness::BE, GameWordSize::ARCH_32, GamePlatform::XBOX),
        new GameVariant(GameVariantId::IW4_PC32, GameId::IW4, "pc32", GameEndianness::LE, GameWordSize::ARCH_32, GamePlatform::PC),
        new GameVariant(GameVariantId::IW4_PC64, GameId::IW4, "pc64", GameEndianness::LE, GameWordSize::ARCH_64, GamePlatform::PC),
        new GameVariant(GameVariantId::IW4_XBOX, GameId::IW4, "xbox", GameEndianness::BE, GameWordSize::ARCH_32, GamePlatform::XBOX),
        new GameVariant(GameVariantId::IW5_PC32, GameId::IW5, "pc32", GameEndianness::LE, GameWordSize::ARCH_32, GamePlatform::PC),
        new GameVariant(GameVariantId::IW5_PC64, GameId::IW5, "pc64", GameEndianness::LE, GameWordSize::ARCH_64, GamePlatform::PC),
        new GameVariant(GameVariantId::IW5_XBOX, GameId::IW5, "xbox", GameEndianness::BE, GameWordSize::ARCH_32, GamePlatform::XBOX),
        new GameVariant(GameVariantId::QOS_PC, GameId::QOS, "pc", GameEndianness::LE, GameWordSize::ARCH_32, GamePlatform::PC),
        new GameVariant(GameVariantId::T4_PC, GameId::T4, "pc", GameEndianness::LE, GameWordSize::ARCH_32, GamePlatform::PC),
        new GameVariant(GameVariantId::T5_PC, GameId::T5, "pc", GameEndianness::LE, GameWordSize::ARCH_32, GamePlatform::PC),
        new GameVariant(GameVariantId::T6_PC, GameId::T6, "pc", GameEndianness::LE, GameWordSize::ARCH_32, GamePlatform::PC),
        new GameVariant(GameVariantId::T6_XBOX, GameId::T6, "xbox", GameEndianness::BE, GameWordSize::ARCH_32, GamePlatform::XBOX),
        new GameVariant(GameVariantId::T6_PS3, GameId::T6, "ps3", GameEndianness::LE, GameWordSize::ARCH_32, GamePlatform::PS3),
        new GameVariant(GameVariantId::T6_WIIU, GameId::T6, "wiiu", GameEndianness::LE, GameWordSize::ARCH_32, GamePlatform::WIIU),
    };
    static_assert(std::size(variants) == std::to_underlying(GameVariantId::COUNT));

    assert(std::to_underlying(variantId) < std::to_underlying(GameVariantId::COUNT));
    auto* result = variants[std::to_underlying(variantId)];
    assert(result);

    return result;
}
