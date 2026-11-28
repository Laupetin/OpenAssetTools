#include "StreamedSoundDumperT4.h"

#include "Utils/Logging/Log.h"

#include <filesystem>

using namespace T4;

namespace sound
{
    void StreamedSoundDumperT4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<snd_alias_list_t>& asset)
    {
        const auto* sound = asset.Asset();
        for (auto i = 0; i < sound->count; i++)
        {
            const auto* soundFile = sound->head[i].soundFile;
            if (!soundFile || !soundFile->exists || (soundFile->type != SAT_STREAMED && soundFile->type != SAT_PRIMED))
                continue;

            const auto& filename = soundFile->u.streamSnd.filename;
            if (!filename.name || !*filename.name)
                continue;
            const auto path = (std::filesystem::path("sound") / (filename.dir ? filename.dir : "") / filename.name).generic_string();
            const auto source = context.m_obj_search_path.Open(path);
            if (!source.IsOpen())
            {
                con::warn("Could not find streamed sound data for \"{}\" in \"{}\"", asset.m_name, path);
                continue;
            }
            if (const auto output = context.OpenAssetFile(path))
                *output << source.m_stream->rdbuf();
        }
    }
} // namespace sound
