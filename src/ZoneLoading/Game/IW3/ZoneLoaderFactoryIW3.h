#pragma once

#include "Loading/IZoneLoaderFactory.h"

#include <string>

namespace IW3
{
    class ZoneLoaderFactory final : public IZoneLoaderFactory
    {
    public:
        [[nodiscard]] std::optional<ZoneLoaderInspectionResult> InspectZoneHeader(ZoneDataPeeking& filePeek) const override;
        [[nodiscard]] std::unique_ptr<ZoneLoader> CreateLoaderForHeader(ZoneDataPeeking& filePeek,
                                                                        const std::string& fileName,
                                                                        std::optional<std::unique_ptr<ProgressCallback>> progressCallback) const override;
    };
} // namespace IW3
