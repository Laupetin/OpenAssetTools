#include "FxEffectDefDumperIW3.h"

#include "FxEffectDefWriterIW3.h"

#include <format>

using namespace IW3;

namespace fx
{
    void EffectDefDumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetFx::Type>& asset)
    {
        const auto* effect = asset.Asset();
        if ((effect->elemDefCountLooping > 0 || effect->elemDefCountOneShot > 0) && !effect->elemDefs)
        {
            con::error("FxEffectDef \"{}\" has element counts but no element definitions", asset.m_name);
            return;
        }

        const auto assetFile = context.OpenAssetFile(std::format("fx/{}.efx", asset.m_name));
        if (!assetFile)
            return;

        const auto writer = CreateEffectDefWriterIW3(*assetFile);
        writer->Write(*effect);
    }
} // namespace fx
