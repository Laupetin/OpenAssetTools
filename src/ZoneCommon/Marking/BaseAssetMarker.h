#pragma once

#include "Game/IAsset.h"
#include "Marking/AssetVisitor.h"
#include "Zone/ZoneTypes.h"

#include <type_traits>

class BaseAssetMarker
{
protected:
    explicit BaseAssetMarker(AssetVisitor& visitor);

    template<AssetDefinition Asset_t> void Mark_Dependency(std::add_lvalue_reference_t<std::add_pointer_t<typename Asset_t::Type>> asset)
    {
        const auto result = m_visitor.Visit_Dependency(Asset_t::EnumEntry, AssetName<Asset_t>(*asset));
        if (result.has_value())
            asset = static_cast<std::add_pointer_t<typename Asset_t::Type>>((*result)->m_ptr);
    }

    void Mark_ScriptString(scr_string_t& scriptString) const;
    void MarkArray_ScriptString(scr_string_t* scriptStringArray, size_t count) const;

    void Mark_IndirectAssetRef(asset_type_t assetType, const char* assetName) const;
    void MarkArray_IndirectAssetRef(asset_type_t assetType, const char** assetNames, size_t count) const;

    AssetVisitor& m_visitor;
};

template<AssetDefinition> struct AssetMarkerWrapper
{
    // using WrapperClass = WrapperClass;
};

template<typename AssetType> using AssetMarker = AssetMarkerWrapper<AssetType>::WrapperClass;

#define DEFINE_MARKER_CLASS_FOR_ASSET(asset, markerClass)                                                                                                      \
    template<> struct AssetMarkerWrapper<asset>                                                                                                                \
    {                                                                                                                                                          \
        using WrapperClass = markerClass;                                                                                                                      \
    };
