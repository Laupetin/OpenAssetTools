#pragma once

#include "Game/IGame.h"
#include "Zone/ZoneTypes.h"

#include <optional>
#include <string>
#include <unordered_map>

class IAssetNameResolver
{
public:
    IAssetNameResolver() = default;
    virtual ~IAssetNameResolver() = default;
    IAssetNameResolver(const IAssetNameResolver& other) = default;
    IAssetNameResolver(IAssetNameResolver&& other) noexcept = default;
    IAssetNameResolver& operator=(const IAssetNameResolver& other) = default;
    IAssetNameResolver& operator=(IAssetNameResolver&& other) noexcept = default;

    [[nodiscard]] virtual GameId GetGameId() const = 0;
    [[nodiscard]] virtual std::optional<asset_type_t> GetAssetTypeByName(const std::string& assetTypeName) const = 0;
    [[nodiscard]] virtual std::optional<const char*> GetAssetTypeName(asset_type_t assetType) const = 0;

    static const IAssetNameResolver* GetResolverForGame(GameId game);
};

class HashMapBasedAssetNameResolver : public IAssetNameResolver
{
public:
    [[nodiscard]] std::optional<asset_type_t> GetAssetTypeByName(const std::string& assetTypeName) const override;

protected:
    void AddAssetTypeName(asset_type_t assetType, std::string name);

private:
    std::unordered_map<std::string, asset_type_t> m_asset_types_by_name;
};
