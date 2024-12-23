#include "AssetCreationResult.h"

AssetCreationResult AssetCreationResult::Success(XAssetInfoGeneric* assetInfo)
{
    return AssetCreationResult(true, assetInfo);
}

AssetCreationResult AssetCreationResult::Failure()
{
    return AssetCreationResult(true, nullptr);
}

AssetCreationResult AssetCreationResult::NoAction()
{
    return AssetCreationResult(false, nullptr);
}

bool AssetCreationResult::HasBeenSuccessful() const
{
    return m_taken_action && m_asset_info != nullptr;
}

bool AssetCreationResult::HasTakenAction() const
{
    return m_taken_action;
}

bool AssetCreationResult::HasFailed() const
{
    return m_taken_action && m_asset_info == nullptr;
}

XAssetInfoGeneric* AssetCreationResult::GetAssetInfo() const
{
    return m_asset_info;
}

AssetCreationResult::AssetCreationResult(const bool takenAction, XAssetInfoGeneric* assetInfo)
    : m_taken_action(takenAction),
      m_asset_info(assetInfo)
{
}
