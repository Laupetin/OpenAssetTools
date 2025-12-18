#pragma once

#include "Game/IAsset.h"
#include "Marking/AssetVisitor.h"
#include "Zone/ZoneTypes.h"

#include <type_traits>

class BaseAssetMarker
{
protected:
    explicit BaseAssetMarker(AssetVisitor& visitor);

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

template<typename AssetType> struct AssetMarkerWrapper
{
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);
    // using WrapperClass = WrapperClass;
};

template<typename AssetType> using AssetMarker = AssetMarkerWrapper<AssetType>::WrapperClass;

#define DEFINE_MARKER_CLASS_FOR_ASSET(asset, markerClass)                                                                                                      \
    template<> struct AssetMarkerWrapper<asset>                                                                                                                \
    {                                                                                                                                                          \
        static_assert(std::is_base_of_v<IAssetBase, asset>);                                                                                                   \
        using WrapperClass = markerClass;                                                                                                                      \
    };
