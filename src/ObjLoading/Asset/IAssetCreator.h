#pragma once

#include "AssetCreationContext.h"
#include "Pool/XAssetInfo.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/ZoneTypes.h"

#include <optional>
#include <string>

class AssetCreationResult
{
public:
    static AssetCreationResult Success(XAssetInfoGeneric* assetInfo);
    static AssetCreationResult Failure();
    static AssetCreationResult NoAction();

    [[nodiscard]] bool HasTakenAction() const;
    [[nodiscard]] bool HasFailed() const;
    [[nodiscard]] XAssetInfoGeneric* GetAssetInfo() const;

private:
    AssetCreationResult(bool takenAction, XAssetInfoGeneric* assetInfo);

    bool m_taken_action;
    XAssetInfoGeneric* m_asset_info;
};

class IAssetCreator
{
public:
    IAssetCreator() = default;
    virtual ~IAssetCreator() = default;
    IAssetCreator(const IAssetCreator& other) = default;
    IAssetCreator(IAssetCreator&& other) noexcept = default;
    IAssetCreator& operator=(const IAssetCreator& other) = default;
    IAssetCreator& operator=(IAssetCreator&& other) noexcept = default;

    [[nodiscard]] virtual std::optional<asset_type_t> GetHandlingAssetType() const = 0;
    virtual AssetCreationResult
        CreateAsset(const std::string& assetName, ISearchPath& searchPath, MemoryManager& memory, AssetCreationContext& context) const = 0;
    virtual void FinalizeZone(AssetCreationContext& context) const = 0;
};
