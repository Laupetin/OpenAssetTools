#pragma once

#include "Game/AutoSearchPaths.h"

class AutoSearchPathsQOS final : public AutoSearchPaths
{
protected:
    [[nodiscard]] const std::vector<std::string>& RecognizedZoneDirs() const override;
    [[nodiscard]] const std::vector<std::string>& AdditionalSearchPaths() const override;
};
