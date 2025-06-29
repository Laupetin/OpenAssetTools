#include "NormalizedJson.h"

#include <nlohmann/json.hpp>

using namespace nlohmann;

std::string JsonNormalized(const std::string& str0)
{
    const json j0 = json::parse(str0);
    return j0.dump(4);
}
