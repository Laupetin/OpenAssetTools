#pragma once

#include "GameLanguage.h"
#include "IAsset.h"
#include "Zone/ZoneTypes.h"

#include <cstdint>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <vector>

enum class GameId : std::uint8_t
{
    IW3,
    IW4,
    IW5,
    QOS,
    T4,
    T5,
    T6,

    COUNT
};

typedef std::uint8_t GameVariantId;

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

static constexpr const char* GameId_Names[]{
    "IW3",
    "IW4",
    "IW5",
    "QOS",
    "T4",
    "T5",
    "T6",
};
static_assert(std::extent_v<decltype(GameId_Names)> == static_cast<unsigned>(GameId::COUNT));

class IGameVariant
{
public:
    IGameVariant() = default;
    virtual ~IGameVariant() = default;
    IGameVariant(const IGameVariant& other) = default;
    IGameVariant(IGameVariant&& other) noexcept = default;
    IGameVariant& operator=(const IGameVariant& other) = default;
    IGameVariant& operator=(IGameVariant&& other) noexcept = default;

    [[nodiscard]] virtual GameVariantId GetId() const = 0;
    [[nodiscard]] virtual const std::string& GetName() const = 0;
    [[nodiscard]] virtual GameEndianness GetEndianness() const = 0;
    [[nodiscard]] virtual GameWordSize GetWordSize() const = 0;
    [[nodiscard]] virtual GamePlatform GetPlatform() const = 0;
};

class GameVariant : public IGameVariant
{
public:
    GameVariant(GameVariantId id, std::string name, GameEndianness endianness, GameWordSize wordSize, GamePlatform platform);

    [[nodiscard]] GameVariantId GetId() const override;
    [[nodiscard]] const std::string& GetName() const override;
    [[nodiscard]] GameEndianness GetEndianness() const override;
    [[nodiscard]] GameWordSize GetWordSize() const override;
    [[nodiscard]] GamePlatform GetPlatform() const override;

private:
    GameVariantId m_id;
    std::string m_name;
    GameEndianness m_endianness;
    GameWordSize m_word_size;
    GamePlatform m_platform;
};

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

    [[nodiscard]] virtual const IGameVariant* GetVariantById(GameVariantId id) const = 0;
    [[nodiscard]] virtual const std::vector<IGameVariant*>& GetVariants() const = 0;

    [[nodiscard]] virtual asset_type_t GetAssetTypeCount() const = 0;
    [[nodiscard]] virtual std::optional<const char*> GetAssetTypeName(asset_type_t assetType) const = 0;
    [[nodiscard]] virtual std::optional<asset_type_t> FindAssetTypeByName(const std::string& potentialAssetTypeName) const = 0;

    [[nodiscard]] virtual asset_type_t GetSubAssetTypeCount() const = 0;
    [[nodiscard]] virtual std::optional<const char*> GetSubAssetTypeName(asset_type_t subAssetType) const = 0;

    static IGame* GetGameById(GameId gameId);
};

class AbstractGame : public IGame
{
public:
    AbstractGame(const char* const* assetTypeNames, asset_type_t assetTypeCount, const char* const* subAssetTypeNames, asset_type_t subAssetTypeCount);

    [[nodiscard]] const std::vector<GameLanguagePrefix>& GetLanguagePrefixes() const override;

    [[nodiscard]] const IGameVariant* GetVariantById(GameVariantId id) const override;

    [[nodiscard]] asset_type_t GetAssetTypeCount() const override;
    [[nodiscard]] std::optional<const char*> GetAssetTypeName(asset_type_t assetType) const override;
    [[nodiscard]] std::optional<asset_type_t> FindAssetTypeByName(const std::string& potentialAssetTypeName) const override;

    [[nodiscard]] asset_type_t GetSubAssetTypeCount() const override;
    [[nodiscard]] std::optional<const char*> GetSubAssetTypeName(asset_type_t subAssetType) const override;

protected:
    template<AssetDefinition Asset_t> void AddAssetTypeNameAlias(const std::string& assetTypeName)
    {
        AddAssetTypeNameAlias(Asset_t::EnumEntry, assetTypeName);
    }

private:
    void AddAssetTypeNameAlias(asset_type_t assetType, const std::string& assetTypeName);

    const char* const* m_asset_type_names;
    asset_type_t m_asset_type_count;

    const char* const* m_sub_asset_type_names;
    asset_type_t m_sub_asset_type_count;

    std::unordered_map<std::string, asset_type_t> m_asset_type_name_lookup;
};
