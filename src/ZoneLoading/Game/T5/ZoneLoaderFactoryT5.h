#pragma once

#include "Loading/IZoneLoaderFactory.h"

#include <string>

namespace T5
{
    class ZoneLoaderFactory final : public IZoneLoaderFactory
    {
    public:
        [[nodiscard]] std::optional<ZoneLoaderInspectionResult> InspectZoneHeader(const ZoneHeader& header) const override;
        [[nodiscard]] std::unique_ptr<ZoneLoader> CreateLoaderForHeader(const ZoneHeader& header,
                                                                        const std::string& fileName,
                                                                        std::optional<std::unique_ptr<ProgressCallback>> progressCallback) const override;
    };
} // namespace T5
