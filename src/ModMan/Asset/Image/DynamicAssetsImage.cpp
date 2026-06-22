#include "DynamicAssetsImage.h"

#include "Context/ModManContext.h"
#include "Game/CommonAsset.h"
#include "Image/Compression/ImageDecompressor.h"
#include "Image/DdsWriter.h"
#include "Image/ImageToCommonConverter.h"
#include "Image/TextureConverter.h"
#include "Pool/XAssetInfo.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <filesystem>
#include <sstream>
#include <utility>

using namespace image;
namespace fs = std::filesystem;

namespace
{
    bool FindImage(const std::string& zoneName, const std::string& assetName, XAssetInfoGeneric*& outAssetInfo, Zone*& outZone)
    {
        auto& context = ModManContext::Get().m_fast_file;
        const auto loadedZones = context.GetLoadedZones();
        for (const auto& loadedZone : loadedZones.Data())
        {
            const auto& zone = loadedZone->GetZone();
            if (zone.m_name != zoneName)
                continue;

            const auto* assetTypeMapper = ICommonAssetTypeMapper::GetCommonAssetMapperByGame(zone.m_game_id);
            const auto gameAssetType = assetTypeMapper->CommonToGameAssetType(CommonAssetType::IMAGE);
            if (!gameAssetType)
                continue;

            outAssetInfo = zone.m_pools.GetAsset(*gameAssetType, assetName);
            if (outAssetInfo)
            {
                outZone = &loadedZone->GetZone();
                return true;
            }
        }

        return false;
    }

    std::optional<ImageFormatId> NeedsConversionToWebGlSupportedFormat(const ImageFormatId imageFormatId)
    {
        static std::unordered_map<ImageFormatId, ImageFormatId> unsupportedFormatMap{
            {ImageFormatId::BC4,         ImageFormatId::B8_G8_R8   },
            {ImageFormatId::BC5,         ImageFormatId::B8_G8_R8   },
            {ImageFormatId::B8_G8_R8_X8, ImageFormatId::B8_G8_R8   },
            {ImageFormatId::R8_G8_B8,    ImageFormatId::B8_G8_R8   },
            {ImageFormatId::R8_G8_B8_A8, ImageFormatId::B8_G8_R8_A8},
            {ImageFormatId::A8,          ImageFormatId::B8_G8_R8   },
            {ImageFormatId::R8,          ImageFormatId::B8_G8_R8   },
            {ImageFormatId::R8_A8,       ImageFormatId::B8_G8_R8   },
        };

        const auto entry = unsupportedFormatMap.find(imageFormatId);
        if (entry != unsupportedFormatMap.end())
            return entry->second;

        return std::nullopt;
    }

    void ImageDds(const webwindowed::dynamic_asset_request& request, webwindowed::dynamic_asset_response& response)
    {
        const auto imageName = request.get_query("name");
        const auto zoneName = request.get_query("zone");
        if (!imageName.has_value() || imageName->empty() || !zoneName.has_value() || zoneName->empty())
        {
            con::error("Bad dds request (name={} zone={})", imageName.value_or(""), zoneName.value_or(""));
            response.send_response(400);
            return;
        }

        XAssetInfoGeneric* image;
        Zone* zone;
        if (!FindImage(*zoneName, *imageName, image, zone))
        {
            con::warn("Could not find image {} of zone {}", *imageName, *zoneName);
            response.send_response(404);
            return;
        }

        assert(image);
        assert(zone);

        const auto gameName = GameId_Names[std::to_underlying(zone->m_game_id)];
        const auto toCommonConverter = ToCommonConverter::GetForGame(zone->m_game_id);
        if (!toCommonConverter)
        {
            con::error("No image converter for game {}", gameName);
            response.send_response(500);
            return;
        }

        std::unique_ptr<Texture> texture;
        {
            const auto searchPaths = ModManContext::Get().m_fast_file.GetSearchPaths();
            texture = toCommonConverter->Convert(*image, searchPaths.Data());
            if (!texture)
            {
                con::warn("Failed to convert image {} of zone {}", *imageName, *zoneName);
                response.send_response(500);
                return;
            }
        }

        const auto* originalTextureFormat = texture->GetFormat();
        const auto originalTextureFormatId = originalTextureFormat->GetId();
        const auto targetFormatId = NeedsConversionToWebGlSupportedFormat(originalTextureFormatId);
        if (targetFormatId)
        {
            const auto* targetFormat = ImageFormat::GetImageFormatById(*targetFormatId);
            if (originalTextureFormat->GetType() == ImageFormatType::BLOCK_COMPRESSED)
            {
                auto* textureDecompressor = ImageDecompressor::GetDecompressorForFormat(originalTextureFormatId);
                assert(textureDecompressor);

                if (textureDecompressor)
                    texture = textureDecompressor->Decompress(*texture, targetFormat);
            }
            else
            {
                TextureConverter converter(texture.get(), targetFormat);
                auto newTexture = converter.Convert();
                assert(newTexture);

                if (newTexture)
                    texture = std::move(newTexture);
            }
        }

        std::ostringstream ss;
        DdsWriter output;
        output.DumpImage(ss, texture.get());

        const auto data = ss.str();
        response.set_content_type("image/x-direct-draw-surface");
        response.send_response(data.data(), data.size());
    }
} // namespace

namespace image
{
    void RegisterDynamicAssets(webwindowed::asset_handler_plugin& assetHandler)
    {
        assetHandler.add_dynamic_asset(webwindowed::dynamic_asset("image/dds", ImageDds));
    }
} // namespace image
