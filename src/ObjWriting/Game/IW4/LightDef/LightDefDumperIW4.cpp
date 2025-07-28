#include "LightDefDumperIW4.h"

#include "LightDef/LightDefCommon.h"

#include <sstream>

using namespace IW4;
using namespace ::light_def;

namespace IW4::light_def
{
    bool Dumper::ShouldDump(XAssetInfo<GfxLightDef>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxLightDef>* asset)
    {
        const auto* lightDef = asset->Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForAsset(asset->m_name));

        if (!assetFile || lightDef->attenuation.image == nullptr || lightDef->attenuation.image->name == nullptr)
            return;

        auto& stream = *assetFile;

        const auto* imageName = lightDef->attenuation.image->name;
        if (imageName[0] == ',')
            imageName = &imageName[1];

        stream << lightDef->attenuation.samplerState << imageName << static_cast<char>(lightDef->lmapLookupStart);
    }
} // namespace IW4::light_def
