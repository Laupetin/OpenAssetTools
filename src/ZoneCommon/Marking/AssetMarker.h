#pragma once

#include "Game/IAsset.h"
#include "Marking/AssetVisitor.h"
#include "Zone/ZoneTypes.h"

#include <type_traits>

class AssetMarker
{
protected:
    explicit AssetMarker(AssetVisitor& visitor);

    template<typename AssetType> void Mark_Dependency(std::add_lvalue_reference_t<std::add_pointer_t<typename AssetType::Type>> asset)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        const auto result = m_visitor.Visit_Dependency(AssetType::EnumEntry, AssetNameAccessor<AssetType>()(*asset));
        if (result.has_value())
            asset = static_cast<std::add_pointer_t<typename AssetType::Type>>(result->m_ptr);
    }

    void Mark_ScriptString(scr_string_t& scriptString) const;
    void MarkArray_ScriptString(scr_string_t* scriptStringArray, size_t count) const;

    void Mark_IndirectAssetRef(asset_type_t assetType, const char* assetName) const;
    void MarkArray_IndirectAssetRef(asset_type_t assetType, const char** assetNames, size_t count) const;

    AssetVisitor& m_visitor;
};
