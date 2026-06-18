#include "AutoSearchPathsT5.h"

const std::vector<std::string>& AutoSearchPathsT5::RecognizedZoneDirs() const
{
    static std::vector<std::string> recognizedZoneDirs = {
        "zone/Common",
        "zone/English",
        "zone/French",
        "zone/Frenchcan",
        "zone/German",
        "zone/Austrian",
        "zone/Italian",
        "zone/Spanish",
        "zone/British",
        "zone/Russian",
        "zone/Polish",
        "zone/Korean",
        "zone/Japanese",
        "zone/Czech",
        "zone/Fulljap",
    };
    return recognizedZoneDirs;
}

const std::vector<std::string>& AutoSearchPathsT5::AdditionalSearchPaths() const
{
    static std::vector<std::string> additionalSearchPaths = {
        "main",
    };
    return additionalSearchPaths;
}
