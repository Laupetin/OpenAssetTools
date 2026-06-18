#include "DynamicAssetsImage.h"

#include "Context/ModManContext.h"
#include "Game/CommonAsset.h"
#include "Image/DdsWriter.h"
#include "Image/ImageToCommonConverter.h"
#include "Pool/XAssetInfo.h"
#include "SearchPath/IWD.h"
#include "SearchPath/SearchPathFilesystem.h"
#include "SearchPath/SearchPaths.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <filesystem>
#include <sstream>

using namespace image;
namespace fs = std::filesystem;

namespace
{
    bool FindImage(const std::string& zoneName, const std::string& assetName, XAssetInfoGeneric*& outAssetInfo, Zone*& outZone)
    {
        const auto& context = ModManContext::Get().m_fast_file;
        for (const auto& loadedZone : context.m_loaded_zones)
        {
            const auto& zone = *loadedZone->m_zone;
            if (zone.m_name != zoneName)
                continue;

            const auto* assetTypeMapper = ICommonAssetTypeMapper::GetCommonAssetMapperByGame(zone.m_game_id);
            const auto gameAssetType = assetTypeMapper->CommonToGameAssetType(CommonAssetType::IMAGE);
            if (!gameAssetType)
                continue;

            outAssetInfo = zone.m_pools.GetAsset(*gameAssetType, assetName);
            if (outAssetInfo)
            {
                outZone = loadedZone->m_zone.get();
                return true;
            }
        }

        return false;
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
        const auto converter = ToCommonConverter::GetForGame(zone->m_game_id);
        if (!converter)
        {
            con::error("No image converter for game {}", gameName);
            response.send_response(500);
            return;
        }

        SearchPaths searchPaths;

        const auto maybeCommon = converter->Convert(*image, searchPaths);
        if (!maybeCommon)
        {
            con::warn("Failed to convert image {} of zone {}", *imageName, *zoneName);
            response.send_response(500);
            return;
        }

        std::ostringstream ss;
        DdsWriter output;
        output.DumpImage(ss, maybeCommon.get());

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
