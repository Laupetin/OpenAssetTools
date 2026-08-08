#pragma once

#include "Asset/IAssetCreator.h"
#include "Zone/Definition/ZoneDefinition.h"
#include "Zone/Zone.h"

#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <zlib.h>

namespace raw_file
{
    std::string CreateBrandingText(const ZoneDefinition& zoneDefinition, std::string_view targetName, std::string_view version);
    std::string CreateBrandingText(const ZoneDefinition& zoneDefinition, std::string_view targetName);

    namespace detail
    {
        template<typename RawFile, bool Compress>
        RawFile* CreateBrandingRawFile(MemoryManager& memory, const std::string& name, const std::string& brandingText)
        {
            auto* rawFile = memory.Alloc<RawFile>();
            rawFile->name = memory.Dup(name.c_str());
            rawFile->len = static_cast<int>(brandingText.size());

            if constexpr (Compress)
            {
                auto compressedSize = compressBound(static_cast<uLong>(brandingText.size()));
                auto* compressedBuffer = memory.Alloc<char>(compressedSize);

                const auto result = compress2(reinterpret_cast<Bytef*>(compressedBuffer),
                                              &compressedSize,
                                              reinterpret_cast<const Bytef*>(brandingText.data()),
                                              static_cast<uLong>(brandingText.size()),
                                              Z_DEFAULT_COMPRESSION);
                if (result != Z_OK)
                    throw std::runtime_error("Compressing branding rawfile failed");

                rawFile->compressedLen = static_cast<int>(compressedSize);
                rawFile->buffer = reinterpret_cast<decltype(rawFile->buffer)>(compressedBuffer);
            }
            else
            {
                auto* buffer = memory.Alloc<char>(brandingText.size() + 1u);
                std::memcpy(buffer, brandingText.data(), brandingText.size());
                buffer[brandingText.size()] = '\0';

                if constexpr (requires { rawFile->compressedLen; })
                    rawFile->compressedLen = 0;

                if constexpr (requires { rawFile->data.buffer; })
                    rawFile->data.buffer = reinterpret_cast<decltype(rawFile->data.buffer)>(buffer);
                else
                    rawFile->buffer = reinterpret_cast<decltype(rawFile->buffer)>(buffer);
            }

            return rawFile;
        }
    } // namespace detail

    template<AssetDefinition Asset_t, bool Compress> class BrandingAssetCreator final : public IAssetCreator
    {
    public:
        BrandingAssetCreator(MemoryManager& memory, const Zone& zone, const ZoneDefinition& zoneDefinition)
            : m_memory(memory),
              m_zone(zone),
              m_zone_definition(zoneDefinition)
        {
        }

        [[nodiscard]] std::optional<asset_type_t> GetHandlingAssetType() const override
        {
            return std::nullopt;
        }

        AssetCreationResult CreateAsset(const std::string&, AssetCreationContext&) override
        {
            return AssetCreationResult::NoAction();
        }

        void FinalizeZone(AssetCreationContext& context) override
        {
            const auto brandingText = CreateBrandingText(m_zone_definition, m_zone.m_name);
            auto* rawFile = detail::CreateBrandingRawFile<typename Asset_t::Type, Compress>(m_memory, m_zone.m_name, brandingText);
            context.AddAsset<Asset_t>(m_zone.m_name, rawFile);
        }

    private:
        MemoryManager& m_memory;
        const Zone& m_zone;
        const ZoneDefinition& m_zone_definition;
    };

    template<AssetDefinition Asset_t, bool Compress = false>
    std::unique_ptr<IAssetCreator> CreateBrandingAssetCreator(MemoryManager& memory, const Zone& zone, const ZoneDefinition& zoneDefinition)
    {
        return std::make_unique<BrandingAssetCreator<Asset_t, Compress>>(memory, zone, zoneDefinition);
    }
} // namespace raw_file
