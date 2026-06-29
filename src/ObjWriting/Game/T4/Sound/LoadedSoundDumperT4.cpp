#include "LoadedSoundDumperT4.h"

#include "Utils/Logging/Log.h"

#include <cassert>
#include <filesystem>
#include <optional>
#include <string>

using namespace T4;

namespace fs = std::filesystem;

namespace
{
    constexpr auto FOURCC_SIZE = 4uz;
    constexpr auto RIFF_WAVE_HEADER_SIZE = 12uz;
    constexpr auto RIFF_CHUNK_HEADER_SIZE = 8uz;

    constexpr std::string_view RIFF_ID = "RIFF";
    constexpr std::string_view WAVE_FORM_TYPE = "WAVE";
    constexpr std::string_view XWMA_FORM_TYPE = "XWMA";

    [[nodiscard]] bool IsFourCc(const char* data, const std::string_view fourCc)
    {
        return fourCc.size() == FOURCC_SIZE && std::equal(fourCc.begin(), fourCc.end(), data);
    }

    enum class LoadedSoundType : std::uint8_t
    {
        WAV,
        XWMA
    };

    std::string GetExtensionBySoundType(const LoadedSoundType soundType)
    {
        switch (soundType)
        {
        case LoadedSoundType::WAV:
            return ".wav";
        case LoadedSoundType::XWMA:
            return ".xwma";
        }

        assert(false);
        return "";
    }

    std::optional<LoadedSoundType> DetermineLoadedSoundType(const char* data, const size_t dataSize)
    {
        if (dataSize < RIFF_WAVE_HEADER_SIZE)
            return std::nullopt;

        if (!IsFourCc(data, RIFF_ID))
            return std::nullopt;

        if (IsFourCc(&data[RIFF_CHUNK_HEADER_SIZE], WAVE_FORM_TYPE))
            return LoadedSoundType::WAV;

        if (IsFourCc(&data[RIFF_CHUNK_HEADER_SIZE], XWMA_FORM_TYPE))
            return LoadedSoundType::XWMA;

        return std::nullopt;
    }
} // namespace

namespace sound
{
    void LoadedSoundDumperT4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<LoadedSound>& asset)
    {
        const auto* loadedSound = asset.Asset();
        if (!loadedSound->sound.data || loadedSound->sound.data_size <= 0)
            return;

        const auto* data = loadedSound->sound.data;
        const auto dataSize = static_cast<size_t>(loadedSound->sound.data_size);

        const auto soundType = DetermineLoadedSoundType(data, dataSize);
        if (soundType)
        {
            auto assetNameWithProperExtension = fs::path(asset.m_name);
            assetNameWithProperExtension.replace_extension(GetExtensionBySoundType(*soundType));
            const auto assetFile = context.OpenAssetFile(std::format("sound/{}", assetNameWithProperExtension.string()));
            if (!assetFile)
                return;

            assetFile->write(data, static_cast<std::streamsize>(dataSize));
        }
        else
        {
            con::error("Failed to dump loaded sound {} due to unknown sound type", asset.m_name);
        }
    }
} // namespace sound
