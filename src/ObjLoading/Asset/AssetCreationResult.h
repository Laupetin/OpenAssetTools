#pragma once

#include "Pool/XAssetInfo.h"

class AssetCreationResult
{
public:
    static AssetCreationResult Success(XAssetInfoGeneric* assetInfo);
    static AssetCreationResult Failure();
    static AssetCreationResult NoAction();

    [[nodiscard]] bool HasBeenSuccessful() const;
    [[nodiscard]] bool HasTakenAction() const;
    [[nodiscard]] bool HasFailed() const;
    [[nodiscard]] XAssetInfoGeneric* GetAssetInfo() const;

private:
    AssetCreationResult(bool takenAction, XAssetInfoGeneric* assetInfo);

    bool m_taken_action;
    XAssetInfoGeneric* m_asset_info;
};
