#pragma once

#include "XAnim/XAnimCommon.h"

#include <expected>
#include <memory>
#include <string>

namespace xanim
{
    std::expected<std::unique_ptr<CommonXAnimParts>, std::string> LoadCompiledXAnim(std::istream& stream);
}
